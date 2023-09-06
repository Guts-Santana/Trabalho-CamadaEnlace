#include <bits/stdc++.h>
#include "CamadaFisica.hpp"
#include "CamadaEnlace.hpp"

using namespace std;

int codType;
string cacheFluxoRetransmissao;

float errorPorcent = -1;

bool loopRetransmissao = false;
int indexRetransmissao = 1;


void retransmissao() {
    if  (loopRetransmissao == false) {
        cout << "Deseja pedir retransissão: " << endl;
        cout << "0 - Não" << endl;
        cout << "1 - Sim" << endl;
        cout << "2 - Sim repetidamente (loop até receber mensagem correta)" << endl;
        int option;
        cin >> option;

        if (option == 0) { exit(0); } //finalizar codigo
        else if (option == 2) { loopRetransmissao = true; }
    }

    cout << "Pedindo Retransmissão... " << "(" << indexRetransmissao << ")" << endl;;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    indexRetransmissao += 1;
    if (indexRetransmissao == 201) {
        cout << "Limite de Retransmissoes" << endl;
        cout << "Encerrando o Programa" << endl;
        exit (0);
    }
    MeioDeComunicacao(cacheFluxoRetransmissao);    
}

void AplicacaoTransmissora() {
    string mensagem;
    cout << "Digite uma mensagem:" << endl;
    getline(cin, mensagem);
    CamadaDeAplicacaoTransmissora(mensagem);
}


void CamadaDeAplicacaoTransmissora(string msg) {
    string mensagem_binaria;

    // Esse FOR serve para transformar as letras e/ou numeros em bytes.
    for (int i = 0; i < (int)msg.length(); i++) {
        char k = msg[i];
        int byte = (int) k;

        // Aqui é onde o byte se transforma em bit com divisoes.
        for (int n = 7; n >= 0; n--) {
            if ((int)byte / (int)pow(2, n) == 1) {
                byte = byte - (int)pow(2, n);
                mensagem_binaria += '1';
            } else {
                mensagem_binaria += '0';
            }
        }
    }
    CamadaEnlaceDadosTransmissora(mensagem_binaria);
}


void CamadaFisicaTransmissora(string binaria) {
    
    int tipoDeCodificacao;
    cout << "Escolha o tipo de modulação" << endl;
    cout << "0 - Binária" << endl;
    cout << "1 - Manchester" << endl;
    cout << "2 - Bipolar" << endl;
    scanf("%d", &tipoDeCodificacao);
    codType = tipoDeCodificacao;
    
    string fluxoBrutoDeBits;

    switch (tipoDeCodificacao) {
        case 0:  // Binária;
            fluxoBrutoDeBits = CamadaFisicaTransmissoraCodificacaoBinaria(binaria);
            break;
        case 1:  // Manchester;
           fluxoBrutoDeBits = CamadaFisicaTransmissoraCodificacaoManchester(binaria);
           break;
        case 2:  // Bipolar;
            fluxoBrutoDeBits = CamadaFisicaTransmissoraCodificacaoBipolar(binaria);
            break;
    }

    cacheFluxoRetransmissao = fluxoBrutoDeBits;
    MeioDeComunicacao(fluxoBrutoDeBits);
}


void MeioDeComunicacao(string fluxo) {
    if (errorPorcent == -1) { //verificando se um pedido de retransmissao
        int errorPorcentInput;
        cout << "Taxa de erro do meio: ";
        cin >> errorPorcentInput;
        errorPorcent = static_cast<float>(errorPorcentInput);
    }

    string fluxoBrutoDeBitsPontoA, fluxoBrutoDeBitsPontoB;

    fluxoBrutoDeBitsPontoA = fluxo;

    vector<int> numerosArray;

    //Colocando caracteres em um array de inteiros
    for (long unsigned int i=0; i<fluxo.size(); i++) {
        if (fluxo[i] == '-') {
            numerosArray.push_back(-1);
            i += 1;
            continue;    
        }

        int numero = fluxo[i] - '0'; // Converte o caractere para um número inteiro
        numerosArray.push_back(numero);
    }

    int numeroAlteracoes = 0;
    for (int numero : numerosArray) {
        // Criando um gerador de números aleatórios (Mersenne Twister Engine)
        std::random_device rd; // Dispositivo random real (se disponível)
        std::mt19937 gen(rd()); // Semente para o gerador

        // Definindo o intervalo desejado para os números aleatórios
        int min = 1;
        int max = 100;

        // Criando uma distribuição que gera números inteiros no intervalo definido
        std::uniform_int_distribution<int> distribution(min, max);

        // Gerando um número aleatório
        int numero_aleatorio = distribution(gen); 

        int randomValue = numero_aleatorio; 
        if (randomValue <= errorPorcent) {
            if (numero == 1) {
                fluxoBrutoDeBitsPontoB += '0';
            }
            else if (numero == 0){
                fluxoBrutoDeBitsPontoB += '1';
            } else if (numero == -1) {
                if (randomValue <= 50) { fluxoBrutoDeBitsPontoB += '1';}
                else { fluxoBrutoDeBitsPontoB += '0';}
            }
            numeroAlteracoes += 1;

        } else {
            if (numero == 1) {
                fluxoBrutoDeBitsPontoB += '1';
            }
            else if (numero == 0){
                fluxoBrutoDeBitsPontoB += '0';
            }
            else if (numero == -1){
                fluxoBrutoDeBitsPontoB += "-1";
            }
        }
    }

    if (fluxoBrutoDeBitsPontoA == fluxoBrutoDeBitsPontoB) {
        cout << "Meio perfeito" << endl;
    } else {
        cout << "Número de bits alterados: " << numeroAlteracoes << endl;
    }


    CamadaFisicaReceptora(fluxoBrutoDeBitsPontoB);
}

void CamadaFisicaReceptora(string fluxoBrutoDeBitsPontoB) {
    int tipoDeDecodificacao = codType;  // Alterar junto com o tipo de Codificacao;
    string fluxoBrutoDeBits;

    switch (tipoDeDecodificacao) {
        case 0:  //decodificacao Binaria
            fluxoBrutoDeBits = CamadaFisicaReceptoraDecodificacaoBinaria(fluxoBrutoDeBitsPontoB);
            break;
        case 1:  //decodificacao Manchester
           fluxoBrutoDeBits = CamadaFisicaReceptoraDecodificacaoManchester(fluxoBrutoDeBitsPontoB);
           break;
        case 2:  //decodificacao Bipolar;
            fluxoBrutoDeBits = CamadaFisicaReceptoraDecodificacaoBipolar(fluxoBrutoDeBitsPontoB);
            break;
    }

    CamadaEnlaceDadosReceptora(fluxoBrutoDeBits);
}

//Essa função é a responsavel por passar de string com binario para char dos inteiros, e assim ler conforme a tabela ASCII
void CamadaDeAplicacaoReceptora(string fluxoBrutoDeBits) {

    string mensagem;

    for (int i = 0; i < (int)fluxoBrutoDeBits.length(); i=i+8) {
        int soma = 0;

        for (int k = 0; k < 8; k++) {
            soma += ((int)fluxoBrutoDeBits[i+k]-'0')*(int)pow(2, 7 - k);
        }
          mensagem.push_back((char)soma);
    }


    //
    // Verificação e exibição da conversão
    bool conversaoCorreta = true;
    for (int i = 0; i < (int)mensagem.length(); i++) {
        if ((mensagem[i] >= 32 && mensagem[i] <= 127) == false) {
            conversaoCorreta = false;
            break;
        }
    }
    if (conversaoCorreta == false) {
        cout << "Erro na conversao ASCII" << endl;
        cout << "Mensagem com tentativa de decodificação: " << mensagem << endl;
        cout << "Deseja pedir retransmissão?" << endl;
        cout << "0 - Não" << endl << "1 - Sim" << endl;
        
        int option;
        cin >> option;

        if (option == 0) { exit(0);}
        else if (option == 1) {retransmissao();}
    }
    //Fim da verificação ASCII

    
    AplicacaoReceptora(mensagem);
}

void AplicacaoReceptora (string mensagem) {

    cout << "A mensagem recebida foi: " << mensagem << endl;
    exit(0);
}


string CamadaFisicaTransmissoraCodificacaoBinaria(string mensagem) {
    cout << mensagem << endl;
    return mensagem;
}

string CamadaFisicaReceptoraDecodificacaoBinaria(string mensagem){
    return mensagem;
}

string CamadaFisicaTransmissoraCodificacaoBipolar(string mensagem) {

    string mensagem_bipolada;
    int x = 1; //Vai servir para definir qual 1 é positivo e qual é negativo

    for (int i = 0; i < (int)mensagem.length(); i++) {

        if (mensagem[i] == '0') {

            mensagem_bipolada += '0';
        } else {
            if (x == 1) {
                mensagem_bipolada += mensagem[i];
                x = -1; // especifica que o próximo 1 é negativo
            } else {
                mensagem_bipolada += '-';
                mensagem_bipolada += mensagem[i];
                x = 1; // especifica que o próximo é positivo
            }
        }
    }
    cout<<mensagem_bipolada<<endl;

    return mensagem_bipolada;
}
string CamadaFisicaReceptoraDecodificacaoBipolar(string mensagem){

    string mensagem_bipolar_decodificada;
    int x = 1;
    for (int i = 0; i < (int)mensagem.length(); i++) {

        if (mensagem[i] == '-' && x == -1) {
            mensagem_bipolar_decodificada += '1';
            x = 1;
            i++;

        } else if (mensagem[i] == '1' && x == 1){
            mensagem_bipolar_decodificada += '1';
            x = -1;
        } else if ( mensagem[i] == '-' || mensagem[i] == '1'){
            cout << "Erro na codificacao" << endl;
        } else{
            mensagem_bipolar_decodificada += '0';
        }
    }

    return mensagem_bipolar_decodificada;
}


string CamadaFisicaTransmissoraCodificacaoManchester(string mensagem){

    string mensagem_manchester;
 //Se o bit lido na subida do clock for 0, ele irá ler como 1 pra 0 (2 bits)
 //Se o bit lido na subida do clock for 1, ele irá ler como 0 pra 1 (2bits)
    for (int i = 0; i < (int)mensagem.length(); i++) {

        if (mensagem[i]=='0') {
            mensagem_manchester += '0';
            mensagem_manchester += '1';

        } else {

            mensagem_manchester += '1';
            mensagem_manchester += '0';
        }
    }

    
    cout << mensagem_manchester << endl;
    return mensagem_manchester;
}

string CamadaFisicaReceptoraDecodificacaoManchester(string mensagem) {

    string mensagem_manchester_decodificada;

    for (int i = 0; i < (int)mensagem.length(); i=i+2) {
        if (mensagem[i] == '0' && mensagem[i+1] == '1') {

            mensagem_manchester_decodificada += '0';

        } else if (mensagem[i] == '1' && mensagem[i+1] == '0'){

            mensagem_manchester_decodificada += '1';

        } else {
            cout << "Erro na codificacao" << endl;
        }
    }

    return mensagem_manchester_decodificada;

}