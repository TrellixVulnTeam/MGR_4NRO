#include "Tools.h"
#include "Math.h"
#include "Hole.h"

void Collapse(Program* program)
{
	std::vector<Point*> points;
	for (int i = 0; i < program->figures.size(); ++i)
	{
		if (program->figures[i]->GetSelected())
		{
			if (program->figures[i]->figureType != FigureType::Point)
			{
				std::cout << "There sould be only Points selected \n";
				return;
			}
			else
			{
				points.push_back((Point*)program->figures[i]);
			}
		}
	}
	if (points.size() != 2)
	{
		std::cout << "There sould be two Points selected \n";
		return;
	}

	Point* newPoint = new Point();
	newPoint->Initialize(program);
	program->figures.push_back(newPoint);
	auto pos = (points[0]->GetPos() + points[1]->GetPos()) / 2.0f;
	newPoint->MoveTo(pos.x, pos.y, pos.z);

	points[0]->ReplaceInParents(newPoint);
	points[1]->ReplaceInParents(newPoint);

	for (int i = 0; i < points.size(); ++i)
	{
		for (int j = 0; j < program->figures.size(); ++j)
		{
			if (program->figures[j] == points[i])
			{
				program->figures.erase(program->figures.begin() + j);
				points[i]->CleanUp();
				delete points[i];
				break;
			}
		}
	}
}

void CreateGregory(Program* program)
{
	std::vector<BezierPatchC0*> patches;
	for (int i = 0; i < program->figures.size(); ++i)
	{
		if (program->figures[i]->GetSelected())
		{
			if (program->figures[i]->figureType != FigureType::BezierPatchC0)
			{
				std::cout << "There sould be only Bezier Patches C0 selected \n";
				return;
			}
			else
			{
				patches.push_back((BezierPatchC0*)program->figures[i]);
			}
		}
	}

	auto res = CheckIfCanMerge(program, patches);

	if (res.size() > 0)
	{
		for (int i = 0; i < res.size(); ++i)
		{
			auto ress = res[i];
			std::vector<std::vector<std::vector<Point*>>> rr;
			rr.push_back(ress[0].patch);
			rr.push_back(ress[1].patch);
			rr.push_back(ress[2].patch);
			Hole* h = new Hole(rr);
			h->Initialize(program);
			ress[0].bezier->AddHole(h);
			ress[1].bezier->AddHole(h);
			ress[2].bezier->AddHole(h);

			program->figures.push_back((Figure*)h);
			if (!program->allGregorys) break;
		}
	}
	else
	{
		std::cout << "Can't Merge\n";
	}
}

std::vector<std::vector<SinglePatch>> CheckIfCanMerge(Program* program, std::vector<BezierPatchC0*> bezierPatches)
{
	std::vector<std::vector<SinglePatch>> res;
	std::vector<SinglePatch> patches;
	for (int i = 0; i < bezierPatches.size(); ++i)
	{
		auto patches_i = bezierPatches[i]->GetAllPatches();
		patches.insert(patches.end(), patches_i.begin(), patches_i.end());
	}

	for (int i = 0; i < patches.size(); ++i)
		for (int j = i + 1; j < patches.size(); ++j)
			for (int k = j + 1; k < patches.size(); ++k)
			{
				auto merge = CanMerge(patches[i], patches[j], patches[k]);
				res.insert(res.end(), merge.begin(), merge.end());
			}
	return res;
}


std::vector<std::vector<SinglePatch>> CanMerge(SinglePatch patch0, SinglePatch patch1, SinglePatch patch2)
{
	std::vector<std::vector<SinglePatch>> res;
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			for (int k = 0; k < 8; ++k)
			{
				if (patch0.patch[0][3] == patch1.patch[0][0] && patch1.patch[0][3] == patch2.patch[0][0] && patch2.patch[0][3] == patch0.patch[0][0])
				{
					bool isOk = true;
					BezierPatchC0* b1 = patch0.bezier;
					BezierPatchC0* b2 = patch1.bezier;
					BezierPatchC0* b3 = patch2.bezier;

					for (int a = 0; a < b1->holes.size(); ++a)
						for (int b = 0; b < b2->holes.size(); ++b)
							for (int c = 0; c < b3->holes.size(); ++c)
								if (b1->holes[a] == b2->holes[b] && b1->holes[a] == b3->holes[c])
									isOk = false;

					if (isOk)
					{
						auto ress = std::vector<SinglePatch>();
						ress.push_back(patch0);
						ress.push_back(patch1);
						ress.push_back(patch2);
						res.push_back(ress);
					}
				}

				patch2.patch = Swap(patch2.patch);
				if (k % 2 == 1)
					patch2.patch = Rotate(patch2.patch);
			}
			patch1.patch = Swap(patch1.patch);
			if (j % 2 == 1)
				patch1.patch = Rotate(patch1.patch);
		}
		patch0.patch = Swap(patch0.patch);
		if (i % 2 == 1)
			patch0.patch = Rotate(patch0.patch);
	}

	return res;
}

std::vector<std::vector<Point*>> Rotate(std::vector<std::vector<Point*>> patch)
{
	std::vector<std::vector<Point*>>newPatch;

	for (int i = 0; i < 4; ++i)
	{
		std::vector<Point*> line;
		for (int j = 0; j < 4; ++j)
		{
			line.push_back(patch[j][3 - i]);
		}
		newPatch.push_back(line);
	}

	return newPatch;
}

std::vector<std::vector<Point*>> Swap(std::vector<std::vector<Point*>> patch)
{
	std::vector<std::vector<Point*>>newPatch;

	for (int i = 0; i < 4; ++i)
	{
		std::vector<Point*> line;
		for (int j = 0; j < 4; ++j)
		{
			line.push_back(patch[i][3 - j]);
		}
		newPatch.push_back(line);
	}

	return newPatch;
}