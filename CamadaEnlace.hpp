#include <bits/stdc++.h>
#include "CamadaFisica.hpp"

using namespace std;


void retransmitir();

int binarioParaDecimal(int binario);

string decimalToBinary(int decimal);

void CamadaEnlaceDadosReceptora(string quadro);

string CamadaEnlaceDadosReceptoraEnquadramentoContagemDeCaracteres(string mensagem);

string CamadaEnlaceDadosReceptoraEnquadramentoInsercaoDeBytes(string mensagem);

string CamadaEnlaceDadosReceptoraEnquadramento (string quadro);

string CamadaEnlaceDadosReceptoraControleDeErroCodigoDeHamming(string mensagem);

string CamadaEnlaceDadosReceptoraControleDeErroCRC(string mensagem);

string CamadaEnlaceDadosReceptoraControleDeErroBitDeParidadePar(string mensagem);

string CamadaEnlaceReceptoraControleDeErro (string quadro);

string CamadaEnlaceTransmissoraEnquadramentoContagemDeCaracteres(string mensagem);

string CamadaEnlaceTransmissoraEnquadramentoInsercaoDeBytes(string mensagem);

string CamadaEnlaceTransmissoraEnquadramento(string quadro);

string CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(string mensagem);

string CamadaEnlaceDadosTransmissoraControleDeErroCRC(string mensagem);

string CamadaEnlaceDadosTransmissoraControleDeErroCodigoDeHamming(string mensagem);

string CamandaEnlaceTransmissoraControleDeErro(string quadro);

void CamadaEnlaceDadosTransmissora(string quadro);
