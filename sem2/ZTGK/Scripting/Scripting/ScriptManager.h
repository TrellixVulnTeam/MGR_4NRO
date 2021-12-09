#ifndef SCRIPT_MANAGER_HEADER
#define SCRIPT_MANAGER_HEADER

/* System Includes */
#include <vector>

/* Project Includes */
#include "GameObject.h"

namespace duckApp
{
	class ScriptManager
	{
		/* TODO (2.01) Create Vector Of Game Objects That Contain Scripts */
		std::vector<GameObject*> gameObjectsScripts;

		/* TODO (2.02) Create Initialize And Finalize Python Methods */
		void InitializePython();
		void FinalizePython();

		/* TODO (2.03) Create Scripts Changed Method */
		void ScriptsChanged();


	public:
		/* TODO (2.04) Create Constructor And Destructor */
		ScriptManager();
		~ScriptManager();

		/* TODO (2.05) Create AddGameObjectScript Method That Takes GameObject Pointer Argument */
		void AddGameObjectScript(GameObject* gameObject);

		/* TODO (2.06) Create Dispath Methods For OnCreate And OnUpdate */
		/*
			* DispathOnCreate Checks For Script Changes And Dispatches OnCreate Method For All Game Objects
			* DispathOnUpdate Checks For Script Changes And Dispatches OnUpdate Method For All Game Objects
		*/
		void DispatchOnCreate();
		void DispatchOnUpdate();

	};
}

#endif /* SCRIPT_MANAGER_HEADER */
