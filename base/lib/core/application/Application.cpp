#include <core/application/Application.hpp>
#include <core/application/OpenDocumentsFile.hpp>

#include <core/presenter/Presenter.hpp>
#include <core/view/View.hpp>

#include <core/undo/Panel/UndoPanelFactory.hpp>
#include <core/undo/UndoControl.hpp>
#include <QSplashScreen>

using namespace iscore;
#include <QMessageBox>
static Application* application_instance = nullptr;

Application::Application(int& argc, char** argv) :
    NamedObject {"Application", nullptr}
{
    // Application
    // Crashes if put in member initialization list... :(
    m_app = new QApplication{argc, argv};
    ::application_instance = this;

    QPixmap logo{":/iscore.png"};
    QSplashScreen splash(logo, Qt::FramelessWindowHint);
    splash.show();

    m_app->setFont(QFont{":/images/Ubuntu-R.ttf", 10, QFont::Normal});
    this->setParent(m_app);
    this->setObjectName("Application");

    QCoreApplication::setOrganizationName("OSSIA");
    QCoreApplication::setOrganizationDomain("i-score.com");
    QCoreApplication::setApplicationName("i-score");

    qRegisterMetaType<ObjectIdentifierVector> ("ObjectIdentifierVector");
    qRegisterMetaType<Selection>("Selection");

    // Colors
    QPalette scenarPalette;
    scenarPalette.setBrush(QPalette::Background, QColor::fromRgb(37, 41, 48));

    scenarPalette.setBrush(QPalette::WindowText, QColor::fromRgb(222, 0, 0)); // Red
    scenarPalette.setBrush(QPalette::Button, QColor::fromRgb(109,222,0)); // Green
    scenarPalette.setBrush(QPalette::Base, QColor::fromRgb(3, 195, 221)); // Blue
    scenarPalette.setBrush(QPalette::AlternateBase, QColor::fromRgb(179, 179, 179)); // Grey
    scenarPalette.setBrush(QPalette::BrightText, QColor::fromRgb(255,225,0)); // Yellow

    qApp->setPalette(scenarPalette, "ScenarioPalette");


    // Settings
    m_settings = std::make_unique<Settings> (this);

    // MVP
    m_view = new View{this};
    m_presenter = new Presenter{m_view, this};

    // Plugins
    m_pluginManager.reloadPlugins();
    m_pluginManager.addControl(new UndoControl{m_presenter, m_presenter});
    m_pluginManager.addPanel(new UndoPanelFactory);

    loadPluginData();

    // View
    m_view->show();

    // Try to reload if there was a crash
    if(openDocumentsFileExists() && QMessageBox::question(
                m_view,
                tr("Reload?"),
                tr("It seems that i-score previously crashed. Do you wish to reload your work?")) == QMessageBox::Yes)
    {
        m_presenter->restoreDocuments();
    }
    else
    {
        if(!m_pluginManager.m_documentPanelList.empty())
            m_presenter->newDocument(m_pluginManager.m_documentPanelList.front());
    }

    splash.finish(m_view);
}

Application::~Application()
{
    this->setParent(nullptr);
    delete m_presenter;

    if(openDocumentsFileExists())
    {
        QFile f{openDocumentsFilePath()};
        f.remove();
    }
    delete m_app;
}

Application &Application::instance()
{
    return *application_instance;
}

void Application::loadPluginData()
{
    for(auto& set : m_pluginManager.m_settingsList)
    {
        m_settings->setupSettingsPlugin(set);
    }

    for(auto& cmd : m_pluginManager.m_controlList)
    {
        m_presenter->registerPluginControl(cmd);
    }

    std::sort(m_presenter->toolbars().begin(), m_presenter->toolbars().end());
    for(auto& toolbar : m_presenter->toolbars())
    {
        m_view->addToolBar(toolbar.bar);
    }

    for(auto& pnl : m_pluginManager.m_panelList)
    {
        m_presenter->registerPanel(pnl);
    }

    for(auto& pnl : m_pluginManager.m_documentPanelList)
    {
        m_presenter->registerDocumentDelegate(pnl);
    }
}

