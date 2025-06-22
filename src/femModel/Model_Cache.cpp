#include "Model.h"

bool FemModel::loadCachedResults(){

    if(!fs::exists(m_resCachePath)){

        LOG << "** No Cache File found" << endl;
        LOG << "** Calculation ..." << endl;
        LOG << endl;

        return false;
    }

    ByteSequence bs;
    bs.fromFile(m_resCachePath);
    bs.decode(m_encoderKey);

    fileTime ftMesh, ftConstraints, ftMaterial;
    bs.extractMultiple(ftMaterial, ftConstraints, ftMesh);

    if(ftMesh != fs::last_write_time(m_meshPath) ||
        ftConstraints != fs::last_write_time(m_constraintPath) ||
        ftMaterial != fs::last_write_time(m_matPath)){

            LOG << "** Changes made to Recc files" << endl;
            LOG << "** Recalculation ..." << endl;
            LOG << endl;

            return false;
    }

    LOG << "** No Changes made to Recc files" << endl;
    LOG << "** Loading results with " << bs.size() << " bytes from cache - " << endl;
    LOG << endl;

    bs.extractMultiple(
        m_simulationSteps, m_deltaTime, m_simulationTime,
        modelDistance, maxModelExtent, modelExtend, modelCenter,
        m_simulationFrames,
        data_lowerXi, data_upperXi, u_lowerXi, u_upperXi, n_lowerXi, n_upperXi,
        m_samples, m_mean, m_deviation,
        m_isoMesh,
        m_materialIsLinear
        );

    return true;
}

void FemModel::cacheResults(){

    ByteSequence bs;

    //
    bs.insertMultiple(
        m_materialIsLinear,
        m_isoMesh,
        m_deviation, m_mean, m_samples,
        n_upperXi, n_lowerXi, u_upperXi, u_lowerXi, data_upperXi, data_lowerXi,
        m_simulationFrames,
        modelCenter, modelExtend, maxModelExtent, modelDistance,
        m_simulationTime, m_deltaTime, m_simulationSteps
    );

    bs.insertMultiple(fs::last_write_time(m_meshPath), fs::last_write_time(m_constraintPath), fs::last_write_time(m_matPath));
    
    LOG << "** Write Cache to " << m_resCachePath << endl;

    timePoint start = chrono::now();

    bs.encode(m_encoderKey);
    bs.toFile(m_resCachePath);

    LOG << "-- Cache und Encoding " << (chrono::now() - start).count() / 1000000000.0f << " s" << endl;
    LOG << "-- Cached Size " << bs.size() << " bytes" << endl;
    LOG << endl;
}