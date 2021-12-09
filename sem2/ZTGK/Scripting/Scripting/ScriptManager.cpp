/* ScriptManager Class Implementation */
#include "ScriptManager.h"

/* TODO (2.07) Include Python And PyBind11 Headers */
#include <pybind11/embed.h>

namespace py = pybind11;

namespace duckApp
{
	/* TODO (2.08) Implement Constructor And Destructor */
	/*
		* Constructor Initializes Python (Initialize Method)
		* Destructor Finalizes Python (Finalize Method)
	*/
	ScriptManager::ScriptManager()
	{
		this->InitializePython();
	}

	ScriptManager::~ScriptManager()
	{
		this->FinalizePython();
	}

	/* TODO (2.09) Implement InitializePython Method */
	/*
		* Sets Python Home Environment Variable
		* Initializes Python Interpreter
		* Imports All Necessary Modules
	*/
	void ScriptManager::InitializePython()
	{
		wchar_t pythonPath[] = L"../Common/Python";

		Py_SetPythonHome(pythonPath);

		py::initialize_interpreter();

		py::module::import("random");
		py::module::import("API");
		//tbd
	}

	/* TODO (2.10) Implement FinalizePython Method */
	/*
		* Finalize Python Interpreter
	*/
	void ScriptManager::FinalizePython()
	{
		py::finalize_interpreter();
	}


	/* TODO (2.11) Implement ScriptsChanged Method */
	/*
		* Iterate Through All Game Objects With Scripts And Check If Any Has Changed
		* If Any Script Has Been Changed Reinitialize Python Interpreter (Finalize And Initialize)
		* Initialize All Game Object's Scripts One Again
	*/
	void ScriptManager::ScriptsChanged()
	{
		bool scriptsChanged = false;

		for (auto gameObject : this->gameObjectsScripts)
		{
			scriptsChanged |= gameObject->GetPythonScript()->FileChanged();
		}

		if (scriptsChanged)
		{
			this->FinalizePython();
			this->InitializePython();

			for (auto gameObject : this->gameObjectsScripts)
			{
				gameObject->GetPythonScript()->Initialize(gameObject);
			}
		}
	}



	/* TODO (2.12) Implement AddGameObjectsSctipt Method That Pushes Back Game Object Pointer */
	void ScriptManager::AddGameObjectScript(GameObject* gameObject)
	{
		this->gameObjectsScripts.push_back(gameObject);
	}

	/* TODO (2.13) Implement DispatchOnCreate And DispatchOnUpdate Method */
	/*
		* Checks If Script File Has Changed
		* Calls Appropriate Method On Game Object Instance
	*/
	void ScriptManager::DispatchOnCreate()
	{
		this->ScriptsChanged();

		for (auto gameObject : this->gameObjectsScripts)
		{
			gameObject->GetPythonScript()->OnCreate();
		}
	}

	void ScriptManager::DispatchOnUpdate()
	{
		this->ScriptsChanged();

		for (auto gameObject : this->gameObjectsScripts)
		{
			gameObject->GetPythonScript()->OnUpdate();
		}
	}

}
