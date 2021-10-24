#include "Tools.h"
#include "Math.h"

void Clear(std::shared_ptr<Program> program)
{
	int n = program->figures.size();

	for (int i = 0; i < n; ++i)
	{
		if (
			program->figures[i]->figureType != FigureType::Cursor &&
			program->figures[i]->figureType != FigureType::MiddlePoint
			)
		{
			std::shared_ptr<Figure> f = program->figures[i];
			program->figures.erase(program->figures.begin() + i);
			f->CleanUp();
			i--;
			n--;
		}
	}
}
