#include "defines.h"

#include "femModel/Model.h"
#include "GUI/ImGuiStyleDecls.h"
#include "GUI/ImGuiCustomElements.h"
#include "Rendering/CameraMovement.h"
#include "Rendering/CellRenderer.h"

class A{

public:

    A() = default;
    A(std::string aIn) : b(aIn){}

    void toByteSequence(ByteSequence& seq) const {
        
        seq.insertMultiple(a,b);
    }

    void fromByteSequence(ByteSequence& seq){

        seq.extractMultiple(b,a);
    }

    std::string getB(){
        return b;
    }

private:

    int a = 20;
    std::string b = "NULLSTR";
};

// Variante für den Fall das die benötigten Member nicht privat sind
// und Deklaration extern außerhalb der Klasse erfolgen oll
//
// OVERRIDE_BYTESEQUENZ_SERIALIZATION(dynNodeXd<float>,
//     insertMultiple(member.a, member.b),
//     extractMultiple(member.b, member.A);

int main() {

    LOG << std::fixed << std::setprecision(4);
    LOG << endl;

    ByteSequence bs, cs;

    bs += A("Hello World");
    LOG << bs.get<A>().getB() << endl;
}