#include "CellPrefabCache.h"

std::map<prefabIndex, CellPrefab> g_cellPrefabCache = {};

//
prefabIndex cellPrefabCounter = 1;

//
NodeIndex cacheCellPrefab(const std::string& prefabLabel){

    const std::string prefabPath = "../Recc/Cells/" + prefabLabel + ".ISOPARAM";

    for(const auto& [index, cellPref] : g_cellPrefabCache){

        //
        if(cellPref.label == prefabLabel){

            LOG << LOG_BLUE << "** Prefab mit Label " << prefabLabel << " im Cache gefunden";
            return index;
        }
    }

    //
    g_cellPrefabCache.emplace(cellPrefabCounter, prefabPath);
    g_cellPrefabCache[cellPrefabCounter].pID = cellPrefabCounter;

    //
    LOG << LOG_GREEN << "** Element " << g_cellPrefabCache[cellPrefabCounter].label << " erfolgreich unter ID " << +cellPrefabCounter << " in CellPrefab Cache geladen" << endl;

    //
    return cellPrefabCounter++;
}

//
const CellPrefab& getCachedCellPrefab(const prefabIndex& index){

    //
    if(!g_cellPrefabCache.contains(index)){

        //
        ASSERT(TRIGGER_ASSERT, "Cellprefab für angegebenen Index nicht definiert");
    }

    return g_cellPrefabCache[index];
}