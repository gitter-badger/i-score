#include <iscore/plugins/plugincontrol/PluginControlInterface.hpp>

#include <core/application/Application.hpp>
#include <core/application/OpenDocumentsFile.hpp>
#include <core/view/View.hpp>

#include <iscore/plugins/panel/PanelFactory.hpp>
#include <iscore/plugins/panel/PanelPresenter.hpp>
#include <iscore/plugins/panel/PanelModel.hpp>
#include <iscore/plugins/panel/PanelView.hpp>

#include <core/document/DocumentPresenter.hpp>
#include <core/document/DocumentView.hpp>

#include <core/undo/UndoControl.hpp>

#include <QFileDialog>
#include <QSettings>
#include <QSaveFile>
#include <QJsonDocument>

using namespace iscore;

Presenter::Presenter(View* view, QObject* arg_parent) :
    NamedObject {"Presenter", arg_parent},
    m_view {view},
    #ifdef __APPLE__
    m_menubar {new QMenuBar, this}
  #else
    m_menubar {view->menuBar(), this}
  #endif
{
    setupMenus();
    connect(m_view,		&View::insertActionIntoMenubar,
            &m_menubar, &MenubarManager::insertActionIntoMenubar);
    connect(m_view, &View::activeDocumentChanged,
            this,   &Presenter::setCurrentDocument);

    connect(m_view, &View::closeRequested,
            this,   &Presenter::closeDocument);
}

#include <QToolBar>
void Presenter::registerPluginControl(PluginControlInterface* ctrl)
{
    ctrl->setParent(this);

    ctrl->populateMenus(&m_menubar);
    m_toolbars += ctrl->makeToolbars();

    m_controls.push_back(ctrl);
}

void Presenter::registerPanel(PanelFactory* factory)
{
    auto view = factory->makeView(m_view);
    auto pres = factory->makePresenter(this, view);

    m_panelPresenters.push_back({pres, factory});

    m_view->setupPanelView(view);

    for(auto doc : m_documents)
        doc->setupNewPanel(pres, factory);
}

void Presenter::registerDocumentDelegate(DocumentDelegateFactoryInterface* docpanel)
{
    m_availableDocuments.push_back(docpanel);
}

const std::vector<PluginControlInterface *> &Presenter::pluginControls() const
{
    return m_controls;
}

const std::vector<DocumentDelegateFactoryInterface *>& Presenter::availableDocuments() const
{
    return m_availableDocuments;
}

Document *Presenter::currentDocument() const
{
    return m_currentDocument;
}

void Presenter::setCurrentDocument(Document* doc)
{
    m_currentDocument = doc;
    if(doc)
    {
        for(auto& pair : m_panelPresenters)
        {
            m_currentDocument->bindPanelPresenter(pair.first);
        }
    }
    else
    {
        for(auto& pair : m_panelPresenters)
        {
            pair.first->setModel(nullptr);
        }
    }

    for(auto& ctrl : m_controls)
    {
        emit ctrl->documentChanged();
    }
}

#include <QMessageBox>
bool Presenter::closeDocument(Document* doc)
{
    // Warn the user if he might loose data
    if(!doc->commandStack().isAtSavedIndex())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("The document has been modified."));
        msgBox.setInformativeText(tr("Do you want to save your changes?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::Save:
                if(saveDocument(doc))
                    break;
                else
                    return false;
            case QMessageBox::Discard:
                // Do nothing
                break;
            case QMessageBox::Cancel:
                return false;
                break;
            default:
                break;
        }
    }

    // Close operation
    m_view->closeDocument(doc->view());
    m_documents.removeOne(doc);

    setCurrentDocument(m_documents.size() > 0 ? m_documents.last() : nullptr);

    delete doc;
    return true;
}

bool Presenter::saveDocument(Document * doc)
{
    QFileDialog d{m_view, tr("Save")};
    d.setNameFilter(tr("Binary (*.scorebin) ;; JSON (*.scorejson)"));
    d.setConfirmOverwrite(true);
    d.setFileMode(QFileDialog::AnyFile);
    d.setAcceptMode(QFileDialog::AcceptSave);

    if(d.exec())
    {
        auto savename = d.selectedFiles().first();
        auto suf = d.nameFilters();
        if(!savename.isEmpty())
        {
            QSaveFile f{savename};
            f.open(QIODevice::WriteOnly);
            if(savename.indexOf(".scorebin") != -1)
                f.write(doc->saveAsByteArray());
            else
            {
                QJsonDocument json_doc;
                json_doc.setObject(doc->saveAsJson());

                f.write(json_doc.toJson());
            }
        }
        return true;
    }
    return false;
}

void Presenter::loadDocument()
{
    QString loadname = QFileDialog::getOpenFileName(m_view, tr("Open"), QString(), "*.scorebin *.scorejson");

    if(!loadname.isEmpty())
    {
        QFile f {loadname};
        if(f.open(QIODevice::ReadOnly))
        {
            Document* theDoc{};
            if (loadname.indexOf(".scorebin") != -1)
            {
                theDoc = loadDocument(f.readAll(), m_availableDocuments.front());
            }
            else
            {
                auto json = QJsonDocument::fromJson(f.readAll());
                theDoc = loadDocument(json.object(), m_availableDocuments.front());
            }

            if(theDoc)
                theDoc->setCrashCommandfile(new CommandBackupFile{theDoc->commandStack(), theDoc});
        }
    }
}

void Presenter::newDocument(DocumentDelegateFactoryInterface* doctype)
{
    auto doc = new Document{doctype, m_view, this};
    m_documents.push_back(doc);

    for(auto& control: m_controls)
    {
        control->on_newDocument(doc);
    }

    for(auto& panel : m_panelPresenters)
    {
        doc->setupNewPanel(panel.first, panel.second);
    }

    m_view->addDocumentView(doc->view());

    setCurrentDocument(doc);


    // Save the initial state of the document
    QSettings s{iscore::openDocumentsFilePath(), QSettings::IniFormat};

    auto existing_files = s.value("iscore/docs").toMap();
    existing_files.insert(doc->crashDataFile().fileName(),
                          doc->crashCommandFile().fileName());
    s.setValue("iscore/docs", existing_files);

    doc->crashDataFile().write(doc->saveAsByteArray());
    doc->crashDataFile().flush();
}

Document* Presenter::loadDocument(const QVariant& data,
                                  DocumentDelegateFactoryInterface* doctype)
{
    Document* doc = nullptr;
    bool inserted = false;
    try
    {
        auto doc = new Document{data, doctype, m_view, this};
        m_documents.push_back(doc);
        inserted = true;

        for(auto& control: m_controls)
        {
            control->on_loadedDocument(doc);
        }

        // TODO same for panels.

        m_view->addDocumentView(doc->view());

        setCurrentDocument(doc);

        doc->crashDataFile().write(doc->saveAsByteArray());
        doc->crashDataFile().flush();

        return doc;
    }
    catch(std::runtime_error& e)
    {
        if(inserted)
        {
            m_documents.removeOne(doc);
            delete doc;
        }

        setCurrentDocument(m_documents.empty() ? nullptr : m_documents.first());
        QMessageBox::warning(m_view, QObject::tr("Error"), e.what());
        return nullptr;
    }
}

void Presenter::restoreDocuments()
{
    QSettings s{iscore::openDocumentsFilePath(), QSettings::IniFormat};

    auto existing_files = s.value("iscore/docs").toMap();
    for(const auto& element : existing_files.keys())
    {
        QFile data_file{element};
        if(data_file.exists())
        {
            data_file.open(QFile::ReadOnly);

            auto doc = loadDocument(data_file.readAll(), m_availableDocuments.front());
            if(doc)
            {
                QFile command_file(existing_files[element].toString());
                if(command_file.exists())
                {
                    command_file.open(QFile::ReadOnly);

                    Deserializer<DataStream> writer(&command_file);
                    writer.writeTo(doc->commandStack());
                }

                doc->setCrashCommandfile(new CommandBackupFile{doc->commandStack(), doc});
            }
        }
    }
}

void Presenter::restoreDocument(const QByteArray &arr)
{
}

iscore::SerializableCommand* Presenter::instantiateUndoCommand(
        const QString& parent_name,
        const QString& name,
        const QByteArray& data)
{
    for(auto& ccmd : m_controls)
    {
        if(ccmd->objectName() == parent_name)
        {
            return ccmd->instantiateUndoCommand(name, data);
        }
    }

    qDebug() << "ALERT: Command" << parent_name << "::" << name << "could not be instantiated.";
    Q_ASSERT(false);

    return nullptr;
}

void Presenter::setupMenus()
{
    ////// File //////
    auto newAct = m_menubar.addActionIntoToplevelMenu(ToplevelMenuElement::FileMenu,
                                                      FileMenuElement::New,
                                                      [&] () { newDocument(m_availableDocuments.front()); });

    newAct->setShortcut(QKeySequence::New);

    // ----------
    m_menubar.addSeparatorIntoToplevelMenu(ToplevelMenuElement::FileMenu,
                                           FileMenuElement::Separator_Load);

    //// Save and load
    // Binary
    auto openAct = m_menubar.addActionIntoToplevelMenu(ToplevelMenuElement::FileMenu,
                                                       FileMenuElement::Load,
                                                       [this]() { loadDocument(); });
    openAct->setShortcut(QKeySequence::Open);

    auto saveAct = m_menubar.addActionIntoToplevelMenu(ToplevelMenuElement::FileMenu,
                                                       FileMenuElement::Save,
                                                       [this]() { saveDocument(currentDocument()); });
    saveAct->setShortcut(QKeySequence::Save);


    //	m_menubar.addActionIntoToplevelMenu(ToplevelMenuElement::FileMenu,
    //										FileMenuElement::SaveAs,
    //										notyet);

    // ----------
    m_menubar.addSeparatorIntoToplevelMenu(ToplevelMenuElement::FileMenu,
                                           FileMenuElement::Separator_Export);

    // ----------
    m_menubar.addSeparatorIntoToplevelMenu(ToplevelMenuElement::FileMenu,
                                           FileMenuElement::Separator_Quit);


    m_menubar.addActionIntoToplevelMenu(ToplevelMenuElement::FileMenu,
                                        FileMenuElement::Quit,
                                        [&] () {
        while(!m_documents.empty())
        {
            bool b = closeDocument(m_documents.last());
            if(!b)
                return;
        }

        qApp->quit();
    });

    ////// View //////
    m_menubar.addMenuIntoToplevelMenu(ToplevelMenuElement::ViewMenu,
                                      ViewMenuElement::Windows);

    ////// Settings //////
    m_menubar.addActionIntoToplevelMenu(ToplevelMenuElement::SettingsMenu,
                                        SettingsMenuElement::Settings,
                                        std::bind(&SettingsView::exec,
                                                  qobject_cast<Application*> (parent())->settings()->view()));

    ////// About /////
    m_menubar.addActionIntoToplevelMenu(ToplevelMenuElement::AboutMenu,
                                        AboutMenuElement::About, [] () { QMessageBox::about(nullptr, tr("About i-score"), tr("With love and sweat from the i-score team.")); });
}
