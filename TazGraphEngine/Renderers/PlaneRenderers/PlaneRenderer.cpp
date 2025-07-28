#include "PlaneRenderer.h"
#include <algorithm>

PlaneRenderer::PlaneRenderer() : 
	_vboInstances(0),

	_glyphs_size(0),
	_triangleGlyphs_size(0),
	_boxGlyphs_size(0),
	_sphereGlyphs_size(0)
{

}

PlaneRenderer::~PlaneRenderer() {

}

void PlaneRenderer::init() {
}

void PlaneRenderer::begin() {
	_glyphs_size = 0;
	_triangleGlyphs_size = 0;
	_boxGlyphs_size = 0;
	_sphereGlyphs_size = 0;
}

void PlaneRenderer::end() {
	createRenderBatches();
}

void PlaneRenderer::initQuadBatch(size_t mSize)
{
	_glyphs_size = mSize;
}

void PlaneRenderer::initTriangleBatch(size_t mSize)
{
	_triangleGlyphs_size = mSize;
}

void PlaneRenderer::initBoxBatch(size_t mSize)
{
	_boxGlyphs_size = mSize;
}

void PlaneRenderer::initSphereBatch(size_t mSize)
{
	_sphereGlyphs_size = mSize;
}


void PlaneRenderer::createRenderBatches() {

	if ((_glyphs_size + _triangleGlyphs_size + _boxGlyphs_size + _sphereGlyphs_size) == 0) {
		return;
	}
}


