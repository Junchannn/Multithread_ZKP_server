#pragma once

#include <NTL/ZZ.h>       // Add this to use ZZ directly
#include <NTL/ZZ_pXFactoring.h>
#include <sstream>
#include <string>
using namespace NTL;


inline std::string ZZToString(const ZZ& num) {
    std::ostringstream oss;
    oss << num;
    return oss.str();
}
// Function to generate a random number in a given range
inline ZZ generate_random_number(const ZZ &a, const ZZ &b) {
    if (a > b) {
        throw std::invalid_argument("Invalid range for generate_random_number: a > b");
    }
    ZZ range = b - a + 1;
    ZZ random_number;
    do {
        random_number = a + RandomBnd(range);
    } while (random_number > b);
    return random_number;
}

// Convert ZZ to string using ZZToString
inline std::string serialize_ZZ(const ZZ& num) {
    return ZZToString(num);
}

// Convert string to ZZ using ZZFromString
inline ZZ unserialize_ZZ(std::string str) {
    return conv<ZZ>(str.c_str());
}

class Prover {
    private:
        ZZ r, w, p, order, g;
    public:
        ZZ y, n;

        // Constructor
        Prover(ZZ w, ZZ p, ZZ g);

        // Generates a parameter for the protocol
        ZZ gen_param();

        // Sets the state based on the challenge e
        ZZ gen_prove(ZZ e);
};

class Verifier {
    public:
        ZZ p, order, e, g, a, y;

        // Constructor
        Verifier();

        // Generates a public key and sets p and order
        void gen_public_key();

        // Generates a challenge e
        void gen_challenge();

        // Verifies the response z from the prover
        bool verify(ZZ z);
};
