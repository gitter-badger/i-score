#include "StoreyModel.hpp"

#include "Document/Constraint/ConstraintModel.hpp"
#include "Document/Constraint/ConstraintContent/ConstraintContentModel.hpp"

#include "Control/ProcessList.hpp"
#include "ProcessInterface/ProcessSharedModelInterface.hpp"
#include "ProcessInterface/ProcessViewModelInterface.hpp"
#include <tools/utilsCPP11.hpp>

#include <QDebug>

QDataStream& operator << (QDataStream& s, const StoreyModel& storey)
{
	s << storey.m_editedProcessId;

	s << (int) storey.m_processViewModels.size();
	for(auto& pvm : storey.m_processViewModels)
	{
		s << pvm->sharedProcessId();
		s << *pvm;
	}

	return s;
}


QDataStream& operator >> (QDataStream& s, StoreyModel& storey)
{
	int editedProcessId;
	s >> editedProcessId;

	int pvm_size;
	s >> pvm_size;
	for(int i = 0; i < pvm_size; i++)
	{
		int sharedprocess_id;
		s >> sharedprocess_id;
		storey.createProcessViewModel(s, sharedprocess_id);
	}

	storey.selectForEdition(editedProcessId);

	return s;
}

StoreyModel::StoreyModel(QDataStream& s, ConstraintContentModel* parent):
	IdentifiedObject{s, "StoreyModel", parent}
{
	s >> *this;
}

StoreyModel::StoreyModel(int id, ConstraintContentModel* parent):
	IdentifiedObject{id, "StoreyModel", parent}
{

}

int StoreyModel::createProcessViewModel(int sharedProcessId, int newProcessViewModelId)
{
	// Search the corresponding process in the parent constraint.
	auto process = parentConstraint()->process(sharedProcessId);
	auto viewmodel = process->makeViewModel(newProcessViewModelId, sharedProcessId, this);

	m_processViewModels.push_back(viewmodel);

	emit processViewModelCreated(viewmodel->id());
	return viewmodel->id();
}

int StoreyModel::createProcessViewModel(QDataStream& s, int sharedProcessId)
{
	// Search the corresponding process in the parent constraint.
	auto process = parentConstraint()->process(sharedProcessId);
	auto viewmodel = process->makeViewModel(s, this);

	m_processViewModels.push_back(viewmodel);

	emit processViewModelCreated(viewmodel->id());
	return viewmodel->id();
}

void StoreyModel::deleteProcessViewModel(int processViewId)
{
	emit processViewModelDeleted(processViewId);

	removeById(m_processViewModels, processViewId);
}

void StoreyModel::selectForEdition(int processViewId)
{
	if(processViewId != m_editedProcessId)
	{
		m_editedProcessId = processViewId;
		emit processViewModelSelected(processViewId);
	}
}

ProcessViewModelInterface*StoreyModel::processViewModel(int processViewModelId) const
{
	return findById(m_processViewModels, processViewModelId);
}

void StoreyModel::on_deleteSharedProcessModel(int sharedProcessId)
{
	// We HAVE to do a copy here because deleteProcessViewModel use the erase-remove idiom.
	auto viewmodels = m_processViewModels;

	for(auto process_vm : viewmodels)
	{
		if(process_vm->sharedProcessId() == sharedProcessId)
		{
			deleteProcessViewModel(process_vm->id());
		}
	}
}

ConstraintModel* StoreyModel::parentConstraint() const
{
	return static_cast<ConstraintModel*>(parent()->parent());
	// TODO Is there a better way to do this ? Without breaking encapsulation ?
	// And without generating another ton of code from constraintmodel to storeymodel ?
}