#include "Renderer.h"
#include <algorithm>

Taz::Renderer::Renderer() :
	_vboInstances(0),

	_lineGlyphs_size(0),
	_triangleGlyphs_size(0),
	_rectangleGlyphs_size(0),
	_boxGlyphs_size(0),
	_sphereGlyphs_size(0)
{

}

Taz::Renderer::~Renderer() {

}

void Taz::Renderer::init() {
}

void Taz::Renderer::begin() {
	_lineGlyphs_size = 0;
	_triangleGlyphs_size = 0;
	_rectangleGlyphs_size = 0;
	_boxGlyphs_size = 0;
	_sphereGlyphs_size = 0;
}
