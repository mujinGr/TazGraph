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

void Taz::Renderer::initQuadBatch(size_t mSize)
{
	_rectangleGlyphs_size = mSize;
}

void Taz::Renderer::initLineBatch(size_t mSize)
{
	_lineGlyphs_size = mSize;
}

void Taz::Renderer::initTriangleBatch(size_t mSize)
{
	_triangleGlyphs_size = mSize;
}

void Taz::Renderer::initBoxBatch(size_t mSize)
{
	_boxGlyphs_size = mSize;
}

void Taz::Renderer::initSphereBatch(size_t mSize)
{
	_sphereGlyphs_size = mSize;
}


