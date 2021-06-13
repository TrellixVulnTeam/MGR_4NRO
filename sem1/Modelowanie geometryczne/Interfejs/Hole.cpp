#include "Hole.h"

Hole::Hole()
{
}

Hole::Hole(std::vector<std::vector<std::vector<Point*>>> _hole)
{
	figureType = FigureType::Hole;
	hole = _hole;
	canMove = false;
}

void Hole::Draw()
{
	if (shouldRecalc) {
		std::vector<std::vector<std::vector<glm::vec3>>> R;
		std::vector<std::vector<std::vector<glm::vec3>>> S;
		std::vector<std::vector<std::vector<glm::vec3>>> T;

		for (int i = 0; i < 3; ++i)
		{
			std::vector<std::vector<glm::vec3>> patchRs;
			for (int j = 0; j < 2; ++j) {
				std::vector<glm::vec3> layer;
				for (int k = 0; k < 3; ++k)
				{
					auto p1 = hole[i][j][k]->GetPos();
					auto p2 = hole[i][j][k + 1]->GetPos();
					layer.push_back((p1 + p2) / 2.0f);
				}
				patchRs.push_back(layer);
			}
			R.push_back(patchRs);
		}

		for (int i = 0; i < 3; ++i)
		{
			std::vector<std::vector<glm::vec3>> patchSs;
			for (int j = 0; j < 2; ++j) {
				std::vector<glm::vec3> layer;
				for (int k = 0; k < 2; ++k)
				{
					auto p1 = R[i][j][k];
					auto p2 = R[i][j][k + 1];
					layer.push_back((p1 + p2) / 2.0f);
				}
				patchSs.push_back(layer);
			}
			S.push_back(patchSs);
		}

		for (int i = 0; i < 3; ++i)
		{
			std::vector<std::vector<glm::vec3>> patchTs;
			for (int j = 0; j < 2; ++j) {
				std::vector<glm::vec3> layer;
				for (int k = 0; k < 1; ++k)
				{
					auto p1 = S[i][j][k];
					auto p2 = S[i][j][k + 1];
					layer.push_back((p1 + p2) / 2.0f);
				}
				patchTs.push_back(layer);
			}
			T.push_back(patchTs);
		}

		std::vector<glm::vec3> P2i;
		std::vector<glm::vec3> Qi;
		for (int i = 0; i < 3; ++i)
		{
			auto t0 = T[i][0][0];
			auto t1 = T[i][1][0];
			P2i.push_back(t0 + (t0 - t1));
			Qi.push_back((3.0f * P2i[i] - t0) / 2.0f);
		}

		glm::vec3 P = (Qi[0] + Qi[1] + Qi[2]) / 3.0f;
		std::vector<glm::vec3> P1i;

		for (int i = 0; i < 3; ++i)
		{
			P1i.push_back((2.0f * Qi[i] + P) / 3.0f);
		}

		std::vector<std::vector<glm::vec3>> innerS;
		std::vector<std::vector<glm::vec3>> innerR;
		std::vector<std::vector<glm::vec3>> innerInner;

		for (int i = 0; i < 3; ++i)
		{
			std::vector<glm::vec3> patchInnerS;
			std::vector<glm::vec3> patchInnerR;
			std::vector<glm::vec3> patchInnerInner;
			patchInnerS.push_back(S[i][0][0] + (S[i][0][0] - S[i][1][0]));
			patchInnerS.push_back(S[i][0][1] + (S[i][0][1] - S[i][1][1]));

			patchInnerR.push_back(R[i][0][0] + (R[i][0][0] - R[i][1][0]));
			patchInnerR.push_back(R[i][0][2] + (R[i][0][2] - R[i][1][2]));

			patchInnerInner.push_back(patchInnerS[0] + (P1i[i] - P2i[i]));
			patchInnerInner.push_back(patchInnerS[1] + (P1i[i] - P2i[i]));

			innerS.push_back(patchInnerS);
			innerR.push_back(patchInnerR);
			innerInner.push_back(patchInnerInner);
		}

		int left, right;
		std::vector<glm::vec3> gregory0;
		std::vector<glm::vec3> gregory1;
		std::vector<glm::vec3> gregory2;

		left = 0;
		right = 1;

#pragma region initGregory0
		//0
		gregory0.push_back(T[left][0][0]);
		//1
		gregory0.push_back(P2i[left]);
		//2
		gregory0.push_back(P1i[left]);
		//3
		gregory0.push_back(P);
		//4
		gregory0.push_back(S[left][0][1]);
		//5
		gregory0.push_back(innerS[left][1]);
		//6
		gregory0.push_back(innerS[left][1]);
		//7
		gregory0.push_back(innerInner[left][1]);
		//8
		gregory0.push_back(innerInner[right][0]);
		//9
		gregory0.push_back(P1i[right]);
		//10
		gregory0.push_back(R[left][0][2]);
		//11
		gregory0.push_back(innerR[left][1]);
		//12
		gregory0.push_back(innerR[right][0]);
		//13
		gregory0.push_back(innerS[right][0]);
		//14
		gregory0.push_back(innerS[right][0]);
		//15
		gregory0.push_back(P2i[right]);
		//16
		gregory0.push_back(hole[right][0][0]->GetPos());
		//17
		gregory0.push_back(R[right][0][0]);
		//18
		gregory0.push_back(S[right][0][0]);
		//19
		gregory0.push_back(T[right][0][0]);
#pragma endregion

		left = 1;
		right = 2;

#pragma region initgregory1
		//0
		gregory1.push_back(T[left][0][0]);
		//1
		gregory1.push_back(P2i[left]);
		//2
		gregory1.push_back(P1i[left]);
		//3
		gregory1.push_back(P);
		//4
		gregory1.push_back(S[left][0][1]);
		//5
		gregory1.push_back(innerS[left][1]);
		//6
		gregory1.push_back(innerS[left][1]);
		//7
		gregory1.push_back(innerInner[left][1]);
		//8
		gregory1.push_back(innerInner[right][0]);
		//9
		gregory1.push_back(P1i[right]);
		//10
		gregory1.push_back(R[left][0][2]);
		//11
		gregory1.push_back(innerR[left][1]);
		//12
		gregory1.push_back(innerR[right][0]);
		//13
		gregory1.push_back(innerS[right][0]);
		//14
		gregory1.push_back(innerS[right][0]);
		//15
		gregory1.push_back(P2i[right]);
		//16
		gregory1.push_back(hole[right][0][0]->GetPos());
		//17
		gregory1.push_back(R[right][0][0]);
		//18
		gregory1.push_back(S[right][0][0]);
		//19
		gregory1.push_back(T[right][0][0]);
#pragma endregion

		left = 2;
		right = 0;

#pragma region initgregory2
		//0
		gregory2.push_back(T[left][0][0]);
		//1
		gregory2.push_back(P2i[left]);
		//2
		gregory2.push_back(P1i[left]);
		//3
		gregory2.push_back(P);
		//4
		gregory2.push_back(S[left][0][1]);
		//5
		gregory2.push_back(innerS[left][1]);
		//6
		gregory2.push_back(innerS[left][1]);
		//7
		gregory2.push_back(innerInner[left][1]);
		//8
		gregory2.push_back(innerInner[right][0]);
		//9
		gregory2.push_back(P1i[right]);
		//10
		gregory2.push_back(R[left][0][2]);
		//11
		gregory2.push_back(innerR[left][1]);
		//12
		gregory2.push_back(innerR[right][0]);
		//13
		gregory2.push_back(innerS[right][0]);
		//14
		gregory2.push_back(innerS[right][0]);
		//15
		gregory2.push_back(P2i[right]);
		//16
		gregory2.push_back(hole[right][0][0]->GetPos());
		//17
		gregory2.push_back(R[right][0][0]);
		//18
		gregory2.push_back(S[right][0][0]);
		//19
		gregory2.push_back(T[right][0][0]);
#pragma endregion
		gp1->UpdateMesh(gregory0);
		gp2->UpdateMesh(gregory1);
		gp3->UpdateMesh(gregory2);
	}
}

void Hole::CleanUp()
{
	ClearPoints();

	for (int i = 0; i < program->figures.size(); ++i)
		if (program->figures[i] == gp1)
			program->figures.erase(program->figures.begin() + i);

	for (int i = 0; i < program->figures.size(); ++i)
		if (program->figures[i] == gp2)
			program->figures.erase(program->figures.begin() + i);

	for (int i = 0; i < program->figures.size(); ++i)
		if (program->figures[i] == gp3)
			program->figures.erase(program->figures.begin() + i);

	gp1->CleanUp();
	delete gp1;
	gp2->CleanUp();
	delete gp2;
	gp3->CleanUp();
	delete gp3;

}

void Hole::ClearPoints()
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 4; ++j)
			for (int k = 0; k < 4; ++k)
			{
				hole[i][j][k]->Unpin(this);
				if (!hole[i][j][k]->HasParent()) hole[i][j][k]->toDel = true;
			}
	int n = program->figures.size();
	for (int i = 0; i < n; ++i)
	{
		if (program->figures[i]->figureType == FigureType::Point)
		{
			if (((Point*)program->figures[i])->toDel)
			{
				delete program->figures[i];
				program->figures.erase(program->figures.begin() + i);
				i--;
				n--;
			}
		}
	}
}

bool Hole::GetGuiInternal(Figure* par)
{
	return toDelete;
}

void Hole::Initialize(Program* _program)
{
	Figure::Initialize(_program);
	program = _program;
	gp1 = new GregoryPatch();
	gp1->Initialize(program);
	gp2 = new GregoryPatch();
	gp2->Initialize(program);
	gp3 = new GregoryPatch();
	gp3->Initialize(program);

	program->figures.push_back(gp1);
	program->figures.push_back(gp2);
	program->figures.push_back(gp3);

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				hole[i][j][k]->AddParent(this);
			}
		}
	}

	Recalc();
}

void Hole::Recalc()
{
	shouldRecalc = true;
}
