#pragma once

#include "defines.h"

template<typename T, size_t DIMENSION>
struct NodeXd {

    // Konstruktoren
    NodeXd() = default;
    NodeXd(const std::vector<T>& Coordinates){

        CRITICAL_ASSERT(Coordinates.size() <= DIMENSION, "Zu viele Werte für Initialisierung der Node übergeben");
        for (size_t coord = 0; coord < Coordinates.size(); ++coord) {
            m_Coordinates[coord] = Coordinates[coord];
        }
    };

    // Koordinaten Abfrage
    const T& operator[](size_t koordIndex) const{

        CRITICAL_ASSERT(koordIndex < DIMENSION, "Abgefragter Wert existiert in Node nicht");
        return m_Coordinates[koordIndex];
    };

    // nicht const Koordinaten abfrage -> Einträge können über zurückgegebene Referenz bearbeitet werden
    T& operator[](size_t koordIndex){

        CRITICAL_ASSERT(koordIndex < DIMENSION, "Abgefragter Wert existiert in Node nicht");
        return m_Coordinates[koordIndex];
    };

    // Logging
    friend std::ostream& operator<<(std::ostream& os, const NodeXd& node) {
        os << "Node-" << DIMENSION << "D {";
        for (size_t i = 0; i < DIMENSION; ++i) {
            if (i > 0) os << ", ";
            os << node.m_Coordinates[i];
        }
        os << "}";
        return os;
    }

    //
    static constexpr size_t dimension = DIMENSION;

private:

    // statisches Array mit erforderlichen Einträgen
    T m_Coordinates[DIMENSION] = {};
};

template<typename T>
struct dynNodeXd {

    // Konstruktoren
    dynNodeXd() = default;
    dynNodeXd(const size_t& dimension, const std::vector<T>& Coordinates)
        : m_dimension(dimension)
    {
        CRITICAL_ASSERT(Coordinates.size() == m_dimension, "Zu viele Werte für Initialisierung der Node übergeben - "
            + std::to_string(m_dimension) + " dimensionale Node hat " + std::to_string(Coordinates.size()) + " Werte übergeben bekommen");
        m_Coordinates.reserve(m_dimension);
        for (size_t coord = 0; coord < Coordinates.size(); ++coord) {
            m_Coordinates.emplace_back(Coordinates[coord]);
        }
    }

    // Koordinaten Abfrage
    const T& operator[](size_t koordIndex) const {
        CRITICAL_ASSERT(koordIndex < m_dimension, "Abgefragter Wert existiert in Node nicht");
        return m_Coordinates[koordIndex];
    }

    // nicht const Koordinaten abfrage -> Einträge können über zurückgegebene Referenz bearbeitet werden
    T& operator[](size_t koordIndex) {
        CRITICAL_ASSERT(koordIndex < m_dimension, "Abgefragter Wert existiert in Node nicht");
        return m_Coordinates[koordIndex];
    }

    // Logging
    friend std::ostream& operator<<(std::ostream& os, const dynNodeXd<T>& node) {
        os << "Node-" << node.m_dimension << "D {";
        for (size_t i = 0; i < node.m_dimension; ++i) {
            if (i > 0) os << ", ";
            os << node.m_Coordinates[i];
        }
        os << "}";
        return os;
    }

    size_t getDimension() const{
        return m_dimension;
    }

private:
    size_t m_dimension = 0;
    std::vector<T> m_Coordinates = {};
};