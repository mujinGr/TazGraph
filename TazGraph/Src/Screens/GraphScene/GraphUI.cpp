#include "./Graph.h"
#include <AppScene/AppInterface.h>

void Graph::updateUI(float deltaTime) {

	_graphEditorLayer.update(deltaTime);

	///*_entityComponentController.setConfig(
	//	{
	//	.mousePos = _mainWindowMousePos, 
	//	.displayedEntity = _displayedEntity,
	//	.manager = manager
	//	}
	//);*/
	//_entityComponentController.update(deltaTime);

	///*_hoverEntityPanel.setConfig
	//({
	//	.mousePos = _app->_inputManager.getMouseCoords(),
	//	.hoveredEntity = _onHoverEntity,
	//	.manager = manager
	//	});*/
}

void Graph::drawUI() {

	_graphEditorLayer.setConfig(
		{
		.scene = this,
		.viewportMousePosition = _viewportMousePosition,

		.viewportFramebuffer  = &_viewportFramebuffer,
		.minimapFramebuffer  = &_minimapFramebuffer,

		.viewportPos  = &_viewportPos,
		.viewportSize  = &_viewportSize,

		.selectionWindowStartPos  = _selectionWindowStartPos,
		.selectionWindowCurrentPos  = _selectionWindowCurrentPos,

		.selectedEntities = &_selectedEntities,

		.map  = map,

		.onHoverEntity  = _onHoverEntity,
		.displayedEntity  = _displayedEntity,

		.sceneManagerActive  = _sceneManagerActive
		}
		);

	_graphEditorLayer.OnImGuiRender();

}
