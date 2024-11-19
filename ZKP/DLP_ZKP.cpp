#include "DLP_ZKP.h"



// Prover class constructor
Prover::Prover(ZZ w, ZZ p, ZZ g) : w(w), p(p), g(g) {
    this->order = p - 1;
    this->y = PowerMod(g, w, p);
}

// Prover methods
ZZ Prover::gen_param() {
    this->r = generate_random_number(g, this->order);
    return PowerMod(this->g, this->r, this->p);
}

ZZ Prover::gen_prove(ZZ e) {
    ZZ z = this->r + e * this->w;
    rem(z, z, this->order);
    return z;
}

// Verifier class constructor
Verifier::Verifier() : g(conv<ZZ>(2)) {}

void Verifier::gen_public_key() {
    GenPrime(this->p, 3072);
    this->order = this->p - 1;
}

void Verifier::gen_challenge() {
    this->e = generate_random_number(this->g, this->order);
}

bool Verifier::verify(ZZ z) {
    ZZ b1 = PowerMod(this->g, z, this->p);
    ZZ b2 = (this->a * PowerMod(this->y, this->e, this->p)) % this->p;
    return b1 == b2;
}
