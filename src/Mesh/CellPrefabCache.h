#pragma once

#include "CellPrefab.h"

// es gibt hier keine statische Optionsauswahl für den Elementtypen
// damit beliebige Prefabs dynamisch geladen werden können
typedef uint8_t prefabIndex;

// Cache für die erstellten Elementvorlagen
inline std::map<prefabIndex, CellPrefab> g_cellPrefabCache = {};

//
inline prefabIndex cellPrefabCounter = 0;

//
inline NodeIndex cacheCellPrefab(const std::string& prefabLabel){

    const std::string prefabPath = "Recc/Cells/" + prefabLabel + ".ISOPARAM";

    for(const auto& [index, cellPref] : g_cellPrefabCache){

        //
        if(cellPref.label == prefabLabel){

            ASSERT(TRIGGER_ASSERT, "Prefab mit Label " + prefabLabel + " existiert bereit im Cache");
            return index;
        }
    }

    //
    g_cellPrefabCache.emplace(cellPrefabCounter, prefabPath);

    //
    LOG << BLUE << "** Element " << g_cellPrefabCache[cellPrefabCounter].label << " erfolgreich unter ID " << +cellPrefabCounter << " in CellPrefab Laufzeit Cache geladen **" << endl;

    //
    return cellPrefabCounter++;
}

//
inline const CellPrefab& getCachedCellPrefab(const prefabIndex& index){

    //
    if(!g_cellPrefabCache.contains(index)){

        //
        ASSERT(TRIGGER_ASSERT, "Cellprefab für angegebenen Index nicht definiert");
    }

    return g_cellPrefabCache[index];
}