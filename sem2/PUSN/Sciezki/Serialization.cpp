#include "Serialization.h"


void Serialize(Program* program)
{
	xml_document <> document;
	xml_node <>* scene = document.allocate_node(node_element, "Scene");
	for (int i = 0; i < program->figures.size(); ++i)
	{
		if (program->figures[i]->figureType == FigureType::Hole ||
			program->figures[i]->figureType == FigureType::GregoryPatch) continue;
		xml_node <>* figure = nullptr;
		std::vector<Point*> points;
		points.push_back(nullptr);
		if (program->figures[i]->figureType == FigureType::Point)
		{
			figure = document.allocate_node(node_element, "Point");
			xml_node <>* position = document.allocate_node(node_element, "Position");
			auto pos = program->figures[i]->GetPos();
			position->append_attribute(document.allocate_attribute("X", document.allocate_string(std::to_string(pos.x).c_str())));
			position->append_attribute(document.allocate_attribute("Y", document.allocate_string(std::to_string(pos.y).c_str())));
			position->append_attribute(document.allocate_attribute("Z", document.allocate_string(std::to_string(pos.z).c_str())));
			figure->append_node(position);
		}
		if (program->figures[i]->figureType == FigureType::BezierCurveC0)
		{
			figure = document.allocate_node(node_element, "BezierC0");
			points = ((SomeCurve*)program->figures[i])->points;
		}
		if (program->figures[i]->figureType == FigureType::BezierCurveC2)
		{
			figure = document.allocate_node(node_element, "BezierC2");
			points = ((SomeCurve*)program->figures[i])->points;
		}
		if (program->figures[i]->figureType == FigureType::InterpolationCurveC2)
		{
			figure = document.allocate_node(node_element, "BezierInter");
			points = ((SomeCurve*)program->figures[i])->points;
		}
		if (program->figures[i]->figureType == FigureType::BezierPatchC0)
		{
			figure = document.allocate_node(node_element, "PatchC0");
			SomePatch* patch = (SomePatch*)program->figures[i];
			figure->append_attribute(document.allocate_attribute("N", document.allocate_string(std::to_string(patch->n).c_str())));
			figure->append_attribute(document.allocate_attribute("M", document.allocate_string(std::to_string(patch->m).c_str())));
			figure->append_attribute(document.allocate_attribute("NSlices", document.allocate_string(std::to_string(patch->splitA).c_str())));
			figure->append_attribute(document.allocate_attribute("MSlices", document.allocate_string(std::to_string(patch->splitB).c_str())));
			points = patch->points;
		}
		if (program->figures[i]->figureType == FigureType::BezierPatchC2)
		{
			figure = document.allocate_node(node_element, "PatchC2");
			SomePatch* patch = (SomePatch*)program->figures[i];
			figure->append_attribute(document.allocate_attribute("N", document.allocate_string(std::to_string(patch->n).c_str())));
			figure->append_attribute(document.allocate_attribute("M", document.allocate_string(std::to_string(patch->m).c_str())));
			figure->append_attribute(document.allocate_attribute("NSlices", document.allocate_string(std::to_string(patch->splitA).c_str())));
			figure->append_attribute(document.allocate_attribute("MSlices", document.allocate_string(std::to_string(patch->splitB).c_str())));
			points = patch->points;
		}
		if (program->figures[i]->figureType == FigureType::Torus)
		{
			figure = document.allocate_node(node_element, "Torus");
			Torus* torus = (Torus*)program->figures[i];
			figure->append_attribute(document.allocate_attribute("MinorRadius", document.allocate_string(std::to_string(torus->r_new).c_str())));
			figure->append_attribute(document.allocate_attribute("MajorRadius", document.allocate_string(std::to_string(torus->R_new).c_str())));
			figure->append_attribute(document.allocate_attribute("MinorSegments", document.allocate_string(std::to_string(torus->n_new).c_str())));
			figure->append_attribute(document.allocate_attribute("MajorSegments", document.allocate_string(std::to_string(torus->m_new).c_str())));

			xml_node <>* position = document.allocate_node(node_element, "Position");
			auto pos = torus->GetPos();
			position->append_attribute(document.allocate_attribute("X", document.allocate_string(std::to_string(pos.x).c_str())));
			position->append_attribute(document.allocate_attribute("Y", document.allocate_string(std::to_string(pos.y).c_str())));
			position->append_attribute(document.allocate_attribute("Z", document.allocate_string(std::to_string(pos.z).c_str())));

			xml_node <>* rotation = document.allocate_node(node_element, "Rotation");
			glm::quat quat = torus->GetRotation();
			rotation->append_attribute(document.allocate_attribute("X", document.allocate_string(std::to_string(quat.x).c_str())));
			rotation->append_attribute(document.allocate_attribute("Y", document.allocate_string(std::to_string(quat.y).c_str())));
			rotation->append_attribute(document.allocate_attribute("Z", document.allocate_string(std::to_string(quat.z).c_str())));
			rotation->append_attribute(document.allocate_attribute("W", document.allocate_string(std::to_string(quat.w).c_str())));

			xml_node <>* scale = document.allocate_node(node_element, "Scale");
			auto sc = torus->GetScale();
			scale->append_attribute(document.allocate_attribute("X", document.allocate_string(std::to_string(sc.x).c_str())));
			scale->append_attribute(document.allocate_attribute("Y", document.allocate_string(std::to_string(sc.y).c_str())));
			scale->append_attribute(document.allocate_attribute("Z", document.allocate_string(std::to_string(sc.z).c_str())));

			figure->append_node(position);
			figure->append_node(rotation);
			figure->append_node(scale);
		}

		if (!(points.size() == 1 && points[0] == nullptr))
		{
			xml_node <>* pointsNode = document.allocate_node(node_element, "Points");
			for (int i = 0; i < points.size(); ++i)
			{
				xml_node <>* pointRef = document.allocate_node(node_element, "PointRef");
				pointRef->append_attribute(document.allocate_attribute("Name", document.allocate_string(points[i]->name)));
				pointsNode->append_node(pointRef);
			}
			figure->append_node(pointsNode);

		}

		figure->append_attribute(document.allocate_attribute("Name", document.allocate_string(program->figures[i]->name)));
		scene->append_node(figure);
	}
	document.append_node(scene);
	std::ofstream myfile;
	myfile.open("serialized.xml");
	myfile << document;
	myfile.close();
}

void Deserialize(Program* program)
{
	Clear(program);

	std::map<std::string, int> indices;
	int k = program->figures.size();
	file<> xmlFile("D:\\studia\\MGR\\sem2\\PUSN\\Sciezki\\somexml.xml");
	xml_document<> doc;
	doc.parse<0>(xmlFile.data());
	auto scene = doc.first_node("Scene");
	auto point = scene->first_node("Point");

	while (point != 0)
	{
		Figure* f = new Point();
		f->Initialize(program);
		glm::vec3 pos;
		auto position = point->first_node("Position");
		float x = atof(position->first_attribute("X")->value());
		float y = atof(position->first_attribute("Y")->value());
		float z = atof(position->first_attribute("Z")->value());
		f->MoveTo(x, y, z);
		auto name = point->first_attribute("Name");
		strcpy_s(f->name, name->value());
		program->figures.push_back(f);
		indices[name->value()] = k;
		k++;

		point = point->next_sibling("Point");
	}

	auto figure = scene->first_node();
	while (figure != 0)
	{
		auto type = figure->name();
		Figure* f = nullptr;
		SomeCurve* sc = nullptr;
		SomePatch* sp = nullptr;
		if (strcmp(type, "Torus") == 0)
		{
			Torus* t = new Torus();
			t->Initialize(program);
			auto minorRadius = atof(figure->first_attribute("MinorRadius")->value());
			auto majorRadius = atof(figure->first_attribute("MajorRadius")->value());
			auto minorSegments = atoi(figure->first_attribute("MinorSegments")->value());
			auto majorSegments = atoi(figure->first_attribute("MajorSegments")->value());
			t->r_new = minorRadius;
			t->R_new = majorRadius;
			t->n_new = minorSegments;
			t->m_new = majorSegments;

			auto position = figure->first_node("Position");
			auto x = atof(position->first_attribute("X")->value());
			auto y = atof(position->first_attribute("Y")->value());
			auto z = atof(position->first_attribute("Z")->value());
			t->MoveTo(x, y, z);

			auto rotation = figure->first_node("Rotation");
			x = atof(rotation->first_attribute("X")->value());
			y = atof(rotation->first_attribute("Y")->value());
			z = atof(rotation->first_attribute("Z")->value());
			auto w = atof(rotation->first_attribute("W")->value());
			glm::quat q = glm::quat(w, x, y, z);
			t->Rotate(q);

			auto scale = figure->first_node("Scale");
			x = atof(scale->first_attribute("X")->value());
			y = atof(scale->first_attribute("Y")->value());
			z = atof(scale->first_attribute("Z")->value());
			t->SetScale(x, y, z);

			f = t;
		}
		if (strcmp(type, "BezierC0") == 0)
		{
			auto bc = new BezierCurveC0();
			bc->drawLine = false;
			sc = bc;
		}
		if (strcmp(type, "BezierC2") == 0)
		{
			auto bc = new BezierCurveC2();
			bc->drawLine = false;
			bc->drawDeBoorLine = false;
			sc = bc;
		}
		if (strcmp(type, "BezierInter") == 0)
		{
			auto bc = new InterpolationCurveC2();
			bc->drawLine = false;
			bc->drawBernsteinPoints = false;
			bc->drawBernsteinLine = false;
			sc = bc;
		}
		if (strcmp(type, "PatchC0") == 0)
		{
			auto n = atof(figure->first_attribute("N")->value());
			auto m = atof(figure->first_attribute("M")->value());
			auto n_slices = atof(figure->first_attribute("NSlices")->value());
			auto m_slices = atof(figure->first_attribute("MSlices")->value());
			sp = new BezierPatchC0(n, m);
			sp->splitA = n_slices;
			sp->splitB = m_slices;
		}
		if (strcmp(type, "PatchC2") == 0)
		{
			auto n = atof(figure->first_attribute("N")->value());
			auto m = atof(figure->first_attribute("M")->value());
			auto n_slices = atof(figure->first_attribute("NSlices")->value());
			auto m_slices = atof(figure->first_attribute("MSlices")->value());
			sp = new BezierPatchC2(n, m);
			sp->splitA = n_slices;
			sp->splitB = m_slices;
		}

		if (sc != nullptr)
		{
			sc->Initialize(program);
			auto points = figure->first_node("Points");
			auto pointRef = points->first_node("PointRef");
			while (pointRef != 0)
			{
				auto name = pointRef->first_attribute("Name")->value();
				int ind = indices[name];
				sc->AddPoint((Point*)program->figures[ind]);
				((Point*)program->figures[ind])->AddParent(sc);
				pointRef = pointRef->next_sibling("PointRef");
			}
			f = sc;
		}

		if (sp != nullptr)
		{
			sp->Initialize(program);
			sp->drawLine = false;
			int dimN;
			int dimM;
			if (sp->figureType == FigureType::BezierPatchC0)
			{
				dimN = 3 * sp->n + 1;
				dimM = 3 * sp->m + 1;
			}
			else
			{
				dimN = 3 + sp->n;
				dimM = 3 + sp->m;
			}
			auto points = figure->first_node("Points");
			auto pointRef = points->first_node("PointRef");
			for (int i = 0; i < dimM; ++i)
				for (int j = 0; j < dimN; ++j)
				{
					auto name = pointRef->first_attribute("Name")->value();
					int ind = indices[name];
					auto point = (Point*)program->figures[ind];
					sp->points.push_back(point);
					point->AddParent(sp);
					if (j > 0)
					{
						sp->pointsLines->AddPoint(sp->points[sp->points.size() - 2]);
						sp->pointsLines->AddPoint(point);
					}
					if (i > 0)
					{
						sp->pointsLines->AddPoint(sp->points[sp->points.size() - dimN - 1]);
						sp->pointsLines->AddPoint(point);
					}
					pointRef = pointRef->next_sibling("PointRef");
				}

			f = sp;
		}


		if (f != nullptr) {
			auto name = figure->first_attribute("Name");
			strcpy_s(f->name, name->value());
			program->figures.push_back(f);
		}
		figure = figure->next_sibling();
	}
}