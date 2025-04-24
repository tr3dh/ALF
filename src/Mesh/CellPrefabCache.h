#pragma once

#include "CellPrefab.h"

// es gibt hier keine statische Optionsauswahl für den Elementtypen
// damit beliebige Prefabs dynamisch geladen werden können
typedef uint8_t prefabIndex;

// Cache für die erstellten Elementvorlagen
extern std::map<prefabIndex, CellPrefab> g_cellPrefabCache;

// counter für die ID Zuweisung für die CellPrefabs
extern prefabIndex cellPrefabCounter;

// cellPrefab wird eine ID zugewiesen und es wird in der g_cellPrefabCache map gecached
// dabei bekommt es eine ID zugewiesen unter der es abjetz abrufbar im Cache bereit liegt
// diese ID wird beim funktionsaufruf zurückgegeben
// wenn das Prefab bereits im Cache liegt wird die ID zurückgegeben unter der es erreichnar ist (map key)
NodeIndex cacheCellPrefab(const std::string& prefabLabel);

//
const CellPrefab& getCachedCellPrefab(const prefabIndex& index);