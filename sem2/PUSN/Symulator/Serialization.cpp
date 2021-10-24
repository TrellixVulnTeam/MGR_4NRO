#include "Serialization.h"


void Serialize(std::shared_ptr<Program> program)
{
	xml_document <> document;
	xml_node <>* scene = document.allocate_node(node_element, "Scene");
	for (int i = 0; i < program->figures.size(); ++i)
	{
		xml_node <>* figure = nullptr;
		std::vector<std::shared_ptr<Point>> points;
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

void Deserialize(std::shared_ptr<Program> program)
{
	Clear(program);

	std::map<std::string, int> indices;
	int k = program->figures.size();
	file<> xmlFile("D:\\studia\\MGR\\sem1\\Modelowanie geometryczne\\Interfejs\\somexml.xml");
	xml_document<> doc;
	doc.parse<0>(xmlFile.data());
	auto scene = doc.first_node("Scene");
	auto point = scene->first_node("Point");

	while (point != 0)
	{
		std::shared_ptr<Figure> f = std::shared_ptr<Point>();
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
		std::shared_ptr<Figure> f;
		std::shared_ptr<SomeCurve> sc;

		if (!!sc)
		{
			sc->Initialize(program);
			auto points = figure->first_node("Points");
			auto pointRef = points->first_node("PointRef");
			while (pointRef != 0)
			{
				auto name = pointRef->first_attribute("Name")->value();
				int ind = indices[name];
				sc->AddPoint(std::dynamic_pointer_cast<Point>(program->figures[ind]));
				(std::dynamic_pointer_cast<Point>(program->figures[ind]))->AddParent(sc);
				pointRef = pointRef->next_sibling("PointRef");
			}
			f = sc;
		}


		if (!!f) {
			auto name = figure->first_attribute("Name");
			strcpy_s(f->name, name->value());
			program->figures.push_back(f);
		}
		figure = figure->next_sibling();
	}
}