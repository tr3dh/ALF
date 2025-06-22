#pragma once

#include <type_traits>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

typedef uint8_t Byte;

template <typename T, template <typename...> class Template>
struct IsInstanceOf : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct IsInstanceOf<Template<Args...>, Template> : std::true_type {};

// die Aufrufe der Member Attribute müssen per member.ATTRIB abgesetzt werden
#define OVERRIDE_BYTESEQUENZ_SERIALIZATION(STRUCTURE, INSERT_LOGIK, EXTRACT_LOGIK) \
    template<>\
    void ByteSequence::insert<STRUCTURE>(const STRUCTURE& member) {\
        INSERT_LOGIK;\
    }\
    \
    template<>\
    void ByteSequence::extract<STRUCTURE>(STRUCTURE& member) {\
        EXTRACT_LOGIK;\
    }

    // die Aufrufe der Member Attribute müssen per member.ATTRIB abgesetzt werden
#define OVERRIDE_BYTESEQUENZ_SERIALIZATION_INLINE(STRUCTURE, INSERT_LOGIK, EXTRACT_LOGIK) \
    template<>\
    inline void ByteSequence::insert<STRUCTURE>(const STRUCTURE& member) {\
        INSERT_LOGIK;\
    }\
    \
    template<>\
    inline void ByteSequence::extract<STRUCTURE>(STRUCTURE& member) {\
        EXTRACT_LOGIK;\
    }

struct ByteSequence;

// Prüft, ob T eine Methode fromByteSequence(ByteSequence&) hat
template<typename, typename = std::void_t<>>
struct hasFromByteSequence : std::false_type {};

template<typename, typename = std::void_t<>>
struct hasToByteSequence : std::false_type {};

template<typename T>
struct hasFromByteSequence<T, std::void_t<
    decltype(std::declval<T>().fromByteSequence(std::declval<ByteSequence&>()))
>> : std::true_type {};

template<typename T>
struct hasToByteSequence<T, std::void_t<
    decltype(std::declval<T>().toByteSequence(std::declval<ByteSequence&>()))
>> : std::true_type {};

// # ByteSequence
// Bei folgender ByteSequenz werden extract und insert solange rekursiv aufgerufen bis die aufgeschlüsselten Member statische Größen aufweisen
// und dann per Memcopy ein/ausgefügt
// so kann jede beliebige Structur zb. vector<vector<vector<vector<int>>>> eingefügt/extrahiert werden so lange das verhalten für vector definiert ist
// zudem können für eigene Klassen über eine eigene Insert/Extract logik die Verarbeitung von Klassenmembern mit dynamischen Atrributen
// auf eine Aufruf in Reihenfolge umgemodelt werden
// Dabei kann auf standardtypen wie std::vector oder std::map zugegriffen werden
// Für diese ist bereits eine Logik implementiert
// Diese Logik definition bzw. Überschreibung des Standardverhaltens kann über das Makro
// ```OVERRIDE_BYTESEQUENZ_SERIALIZATION(STRUCTURE, INSERT_LOGIK, EXTRACT_LOGIK)``` angestellt werden
// die Klassen attribute müssen über member. angesprochen werden und beim Aufruf von Extract/Insert muss invertiert zum insert der Extract
// erfolgen
//
// ```c++
// struct A{int b; char c;};
//
// OVERRIDE_BYTESEQUENZ_SERIALIZATION(A,
//    insertMultiple(member.b, member.c),
//    extractMultiple(member.c, member.b))
// ```
// 
// Wenn die Klasse selbst deklariert wird und auch private Attribute befüllt werden sollen
// kann die Serialisierung auch direkt in der Klassen Deklaration erfolgen
//
// ```c++
// struct A{
// 
// public: // nur bei Klasse nötig
//     
//      void toByteSequence(ByteSequence& seq) const {
//          seq.insertMultiple(a,b);
//          seq.encode(...);
//      }
//
//      void fromByteSequence(ByteSequence& seq){
//          seq.extractMultiple(b,a);
//          seq.decode(...);
//      }
// 
// private:
//      int b; std::string c;
// };
// ```
//
// dabei kann auf sämtliche funktionen der ByteSequenz zugegriffen werden
// zb. über eine Klassen interne Verschlüsselung oder ähnliches
//
// Grundsätzlich muss bei der Implementierung einer Serialisierungen beachtet werden, dass die insert Reihenfolge umgekehrt zur
// extract Reihenfolge ist
//
// Ohne die Umkehrung der Reihenfolge beim extract kommt es zu undefiniertem Verhalten 
struct ByteSequence{

    // Eigentlicher Container für die Daten
    // in diesem Vektor steht die ByteSequenz und der gesamte struct enthält nur ein paar wrapper funktionen
    // die converts, inserts, extracts und weiteres erlauben
    std::vector<Byte> bytes = {};
    
    // Print der Sequenz, unsichtbare chars werden nicht angezeigt
    friend std::ostream& operator<<(std::ostream& os, const ByteSequence& sequence) {
        
        //
        os << "ByteSequence " << sequence.bytes.size() << " bytes [";

        //
        for(const auto& byte : sequence.bytes){

            os << byte;
        }

        //
        os << "]";
        return os;
    }

    // Größe der Sequenz
    size_t size(){
        
        return bytes.size();
    }

    // einfacher convert über Konstruktor
    std::string toString(){

        return std::string(bytes.begin(), bytes.end());
    }

    // einfacher convert über Konstruktor
    void fromString(const std::string& str){

        bytes = std::vector<Byte>(str.begin(), str.end());
    }

    // abspeichern in File
    void toFile(const std::string& file){

        // Parent erstellen wenn nicht vorhanden
        fs::create_directories(fs::path(file).parent_path());

        //
        std::ofstream outFile(file, std::ios::binary);

        //
        if(outFile){
            outFile << toString();
            outFile.close();
        }
    }

    // Laden aus file
    void fromFile(const std::string& file){

        std::ifstream inFile(file, std::ios::binary);

        if(inFile){

            std::ostringstream ss;
            ss << inFile.rdbuf();
            inFile.close();

            fromString(ss.str());
        }
    }

    // Insert mit memcopy der gesamten Inhalt statischer Member in den bytevektor anstellt
    template<typename T>
    void insertStaticMember(const T& member){

        // Sequence Größe cachen und erweiterung des byteVektors um benötigte Größe
        const size_t sequenceSize = bytes.size();
        bytes.resize(sequenceSize + sizeof(T));

        // memcopy aus source objekt in byte Vector
        std::memcpy(bytes.data() + sequenceSize, &member, sizeof(T));
    }

    // Extract mit memcopy der Inhalt in Größe des statischen Members aus byteVektor in Member kopiert und aus
    // dem Vektor löscht
    template<typename T>
    void extractStaticMember(T& member){

        // Prüfen ob sequenz noch genug bytes enthält um den member zu füllen
        RETURNING_ASSERT(bytes.size() >= sizeof(T), "Byte Sequenz enthält nicht genügend bytes für extract",);

        // Sequence Größe cachen und Memcopy von byteVector in referenzierten Member
        const size_t sequenceSize = bytes.size() - sizeof(T);

        //
        std::memcpy(&member, bytes.data() + sequenceSize, sizeof(T));

        //
        bytes.resize(sequenceSize);
    }

    // rekursiver Wrapper der so lange rekursiv die jeweils untergeordnete insert Funktion aufruft bis
    // bis der finale insert über einen statischen Member erfolgen kann
    template<typename T>
    void insert(const T& member){

        if constexpr(IsInstanceOf<T, std::vector>::value){

            // Rückwärts damit Reihenfolge des herausgezogenen Vektors identisch mit der des Ursprungsvektors wird
            // normalerweise wird die Reihenfolge beim insert invertiert, da immmer von hinten in den Vector geschrieben und
            // auch von hinten extrahiert wird 
            for(const auto& elem : std::views::reverse(member)){
                insert(elem);
            }

            // Wichtig damit beim Extract klar ist wie viele Elemente für den Vector in die ByteSequenz gepusht wurden
            insert(member.size());
        }
        else if constexpr(IsInstanceOf<T, std::map>::value){

            //
            for(auto it = member.rbegin(); it != member.rend(); ++it){
                insertMultiple(it->second, it->first);
            }

            //
            insert(member.size());
        }
        // Bei der unordered Map wird die Reihenfolge durch das cachen nicht aufrechterhalten
        else if constexpr(IsInstanceOf<T, std::unordered_map>::value){

            //
            for(const auto& [key,val] : member){
                insertMultiple(val, key);
            }

            //
            insert(member.size());
        }
        else if constexpr(std::is_same_v<T, std::string>){

            //
            for (auto it = member.rbegin(); it != member.rend(); ++it) {
                insert(*it);
            }
            insert(member.size());
        }
        //
        else if constexpr (std::is_same_v<T, char*> || std::is_same_v<T, const char*> ||
                            (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>)){

            //
            insert(std::string(member));
        }
        // Aufruf des inserts für statische member weil Typ zu den Standardgrößen mit statischer Speichergröße zählt
        // oder aus anderen Gründen eine statische Speicher größe aufweist
        else if constexpr(std::is_fundamental<T>::value ||
            std::is_trivially_copyable<T>::value && std::is_standard_layout<T>::value){
                                                            
            insertStaticMember(member);
        }
        else if constexpr (std::is_class<T>::value){

            if constexpr (hasToByteSequence<T>::value) {

                // member funktion aufrufen und bytesequenz als referenz übergeben
                member.toByteSequence(*this);
            } else {
                toByteSequence(member, *this);
            }
        }
        else {

            toByteSequence(member, *this);

            RETURNING_ASSERT(TRIGGER_ASSERT,
                "Einfügen von " + std::string(typeid(T).name())
                + " in Bytesequenz schlägt fehl, da member dynamsiche Attribute aufweist, Verhalten muss deklariert werden",);
        }
    }

    // rekursiver Wrapper der so lange rekursiv die jeweils untergeordnete extract Funktion aufruft bis
    // bis der finale extract über einen statischen Member erfolgen kann 
    template<typename T>
    void extract(T& member){

        if constexpr(IsInstanceOf<T, std::vector>::value){

            size_t vectorSize = get<size_t>();
            member.clear();
            member.resize(vectorSize);

            for(size_t counter = 0; counter < vectorSize; counter++){
                extract(member[counter]);
            }
        }
        else if constexpr(IsInstanceOf<T, std::map>::value || IsInstanceOf<T, std::unordered_map>::value){

            size_t mapSize = get<size_t>();
            member.clear();
            
            for(size_t counter = 0; counter < mapSize; counter++){

                const typename T::key_type key = get<typename T::key_type>();
                member.try_emplace(key);

                extract(member[key]);
            }
        }
        else if constexpr(std::is_same_v<T, std::string>){

            size_t stringSize = get<size_t>();
            member.resize(stringSize);

            //
            for (size_t counter = 0; counter < stringSize; counter++) {
                extract(member[counter]);
            }
        }
        // Aufruf des inserts für statische member weil Typ zu den Standardgrößen mit statischer Speichergröße zählt
        // oder aus anderen Gründen eine statische Speicher größe aufweist
        else if constexpr(std::is_fundamental<T>::value ||
            std::is_trivially_copyable<T>::value && std::is_standard_layout<T>::value){
                                                            
            extractStaticMember(member);
        }
        else if constexpr (std::is_class<T>::value){

            // member funktion aufrufen und bytesequenz als referenz übergeben
            if constexpr (hasFromByteSequence<T>::value) {

                // member funktion aufrufen und bytesequenz als referenz übergeben
                member.fromByteSequence(*this);
            } else {
                fromByteSequence(member, *this);
            }
        }
        else {

            fromByteSequence(member, *this);

            RETURNING_ASSERT(TRIGGER_ASSERT,
                "Einfügen in Bytesequenz schlägt fehl, da member dynamsiche Attribute aufweist, Verhalten muss deklariert werden",);
        }
    }

    // Wrapper der insert für eine Member liste aufruft
    template<typename... Ts>
    void insertMultiple(const Ts&... members) {

        // Fold über Komma
        (insert(members), ...);
    }

    // Wrapper der extract für eine Member liste aufruft
    template<typename... Ts>
    void extractMultiple(Ts&... members){

        // Fold über Komma
        (extract(members), ...);
    }

    // # Shiffre Encoder
    // Codierung über einfache shiffre, also Tokenverschiebung
    // da die höher wertigen bytes beim string convert teilweise
    // für charfolgen und nicht mehr einzelne chars stehen
    // wird der erzeugt string häufig größer als der string für den eigentlichen Vektor
    // >> ineffizient bei Speicherung und wenig sicher
    void encode(const int& shiffreKey){

        for(uint8_t& byte : bytes){
            byte += shiffreKey;
        }
    }

    //
    void decode(const int& shiffreKey){

        encode(-shiffreKey);
    }

    // # XOR Encoder
    // nimmt eine Bytefolge und verodert jeden nten char aus der sequenz
    // mit dem nten char aus dem key
    // Für das verodern wird xor (exkludierendes oder, operator^) verwendet
    // dieses ist reversible
    // also a* = a ^ b --> a = a* ^ b
    // Deshalb kann ein mit dieser Codierung verschlüsselter Bytestream
    // über den erneuten aufruf der Codierungsfunktion decodiert werden
    // wichtig ist dabei nur, dass der Schlüssel der gleiche ist 
    void encode(const uint8_t* key, size_t key_len) {
        for (size_t i = 0; i < bytes.size(); ++i) {
            bytes[i] ^= key[i % key_len];
        }
    }
    void decode(const uint8_t* key, size_t key_len) {
        encode(key, key_len);
    }

    // # String Wrapper für Xor Encoder
    // nimmt string und wandelt ihn in bytefolge für xor encoder um
    void encode(const std::string& key) {

        encode(reinterpret_cast<const uint8_t*>(key.data()), key.size());
    }
    void decode(const std::string& key) {

        encode(key);
    }

    // Lazy syntax Funktionen

    //
    template<typename T>
    void operator +=(const T& member){

        insert(member);
    }

    //
    template<typename T>
    void operator -=(T& member){

        extract(member);
    }

    // Konstruiert member über angegeben Konstruktor Argumente und befüllt ihn dnn
    // braucht im Gegensatz zu extract keine Referenz
    // >> Objekt muss im Vorhinein nicht konstruiert werden sonder kann mit get initialisiert werden
    template<typename T, typename... Args>
    T get(Args&&... args){

        T member(std::forward<Args>(args)...);
        extract(member);

        return member;
    }
};