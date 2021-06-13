#include "Tools.h"
#include "Figure.h"
#include "Point.h"

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

	if (patches.size() != 3)
	{
		std::cout << "There sould be three Patches selected \n";
		return;
	}

	auto res = CheckIfCanMerge(program, patches);

	if (res.size() > 0)
	{
		std::cout << "Can Merge\n";
	}
	else
	{
		std::cout << "Can't Merge\n";
	}
}

std::vector<std::vector<std::vector<Point*>>> CheckIfCanMerge(Program* program, std::vector<BezierPatchC0*> patches)
{

	auto patches0 = patches[0]->GetAllPatches();
	auto patches1 = patches[1]->GetAllPatches();
	auto patches2 = patches[2]->GetAllPatches();

	for (int i = 0; i < patches0.size(); ++i)
		for (int j = 0; j < patches1.size(); ++j)
			for (int k = 0; k < patches2.size(); ++k)
			{
				auto merge = CanMerge(patches0[i], patches1[j], patches2[k]);
				if (merge.size() > 0) return merge;
			}
}


std::vector<std::vector<std::vector<Point*>>> CanMerge(std::vector<std::vector<Point*>> patch0, std::vector<std::vector<Point*>> patch1, std::vector<std::vector<Point*>> patch2)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			for (int k = 0; k < 8; ++k)
			{
				if (patch0[0][3] == patch1[0][0] && patch1[0][3] == patch2[0][0] && patch2[0][3] == patch0[0][0])
				{
					auto res = std::vector<std::vector<std::vector<Point*>>>();
					res.push_back(patch0);
					res.push_back(patch1);
					res.push_back(patch2);
					return res;
				}

				patch2 = Swap(patch2);
				if (k % 2 == 1)
					patch2 = Rotate(patch2);
			}
			patch1 = Swap(patch1);
			if (j % 2 == 1)
				patch1 = Rotate(patch1);
		}
		patch0 = Swap(patch0);
		if (i % 2 == 1)
			patch0 = Rotate(patch0);
	}

	return std::vector<std::vector<std::vector<Point*>>>();
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