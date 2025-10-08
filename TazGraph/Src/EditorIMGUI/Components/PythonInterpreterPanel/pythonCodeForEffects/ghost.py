if 'ghostManager' not in dir():
    class GhostNode:
        def __init__(self, simId, entityId):
            self.simId = simId  # Fixed: was originalSimId
            self.entityId = entityId
    
    class GhostManager:
        def __init__(self):
            self.lastStep = -1
            self.ghostNodes = []  # List[GhostNode]
            self.ghostLinks = []  # List[dict] with "id" and sim info
        
        def clearGhosts(self):
            if self.ghostLinks:
                deletedLinks = deleteEntities(self.ghostLinks)
                print(f"Deleted {deletedLinks} ghost links from previous step")
                self.ghostLinks = []
            
            if self.ghostNodes:
                # Convert GhostNode objects to dicts for deleteEntities
                ghostNodeDicts = [{"id": g.entityId} for g in self.ghostNodes]
                deletedNodes = deleteEntities(ghostNodeDicts)
                print(f"Deleted {deletedNodes} ghost nodes from previous step")
                self.ghostNodes = []
        
        def createGhostNodes(self):
            simNodes = getSimNodes()
            for node in simNodes:
                ghostNode = deepCopyNode(node['simId'], 0.25)
                if ghostNode:
                    # Store GhostNode object
                    self.ghostNodes.append(GhostNode(node['simId'], ghostNode['id']))
                    print(f"Created ghost node for simId={node['simId']}")
        
        def findGhostBySimId(self, simId):
            for g in self.ghostNodes:
                if g.simId == simId:  # Fixed: was g.originalSimId
                    return g
            return None
        
        def createGhostLinks(self):
            simLinks = getSimLinks()
            for link in simLinks:
                fromGhost = self.findGhostBySimId(link['fromId'])
                toGhost = self.findGhostBySimId(link['toId'])
                
                if not fromGhost or not toGhost:
                    print(f"Skipping ghost link {link['simId']} (missing ghost nodes)")
                    continue
                
                # Create new link between ghost nodes
                ghostLink = addLink(fromGhost.entityId, toGhost.entityId, 0.25)
                if ghostLink:
                    # Copy properties from original link
                    ghostLink["originalSimId"] = link["simId"]
                    self.ghostLinks.append(ghostLink)
                    print(f"Created ghost link for simId={link['simId']}")
    
    ghostManager = GhostManager()

# --- Per frame / per update execution ---
currentStep = getCurrentStep()

if ghostManager.lastStep != currentStep and currentStep >= 0:
    print(f"Step changed from {ghostManager.lastStep} → {currentStep}")
    
    # Remove old ghosts
    ghostManager.clearGhosts()
    
    # Create new ghost copies
    ghostManager.createGhostNodes()
    ghostManager.createGhostLinks()
    
    print(f"Created {len(ghostManager.ghostNodes)} ghost nodes, "
          f"{len(ghostManager.ghostLinks)} ghost links")
    
    # Save step
    ghostManager.lastStep = currentStep