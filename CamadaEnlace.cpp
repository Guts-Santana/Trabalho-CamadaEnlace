#include <bits/stdc++.h>
#include "CamadaFisica.hpp"
#include "CamadaEnlace.hpp"

using namespace std;

int espacoInsercaoBytes = 128; //Numero de bits em cada quadro "entre flags"
string flag = "01111110";
string escape = "00011011";

string polinomioGerador = "100000100110000010001110110110111"; //Polinomio gerador do CRC

int tipoDeEnquadramento = 0; // Tipos de enquadramento de acordo com o teste
int tamanhoCabecalhoContagemDeCaracter = 64;     //Tamanho de caracter do cabecalho do quadro

int tipoDeControleDeErro = 0;

void retransmitir() {
    retransmissao();
}

int binarioParaDecimal(int binario) {
    int decimal = 0;
    int potencia = 0;

    while (binario != 0) {
        int digito = binario % 10;
        decimal += digito * std::pow(2, potencia);
        binario /= 10;
        potencia++;
    }

    return decimal;
}


string decimalToBinary(int decimal) {
    if (decimal == 0) {
        return "0"; // Caso especial para o decimal igual a 0
    }

    string binary = "";
    while (decimal > 0) {
        // Adiciona o dígito menos significativo do número decimal à esquerda da string binária
        binary = std::to_string(decimal % 2) + binary;
        decimal /= 2; // Divide o número decimal por 2 para obter o próximo dígito
    }

    return binary;
}

void CamadaEnlaceDadosTransmissora(string quadro) {
    string mensagem;
    mensagem = CamadaEnlaceTransmissoraEnquadramento(quadro);
    mensagem = CamandaEnlaceTransmissoraControleDeErro(mensagem);
    
    CamadaFisicaTransmissora(mensagem);

}

string CamandaEnlaceTransmissoraControleDeErro(string quadro) {
    string mensagemComControle;

    int EnqType;
    cout << "Escolha o controle de erro" << endl;
    cout << "0 - Bit paridade par" << endl;
    cout << "1 - CRC" << endl;
    cout << "2 - Código de Hamming" << endl;
    scanf("%d", &EnqType);
    tipoDeControleDeErro = EnqType;

    switch (tipoDeControleDeErro) {
        case 0: //bit paridade par
            mensagemComControle =  CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(quadro);
            break;
        case 1: //CRC
            mensagemComControle =  CamadaEnlaceDadosTransmissoraControleDeErroCRC(quadro);
            break;
        case 2: //Código de Hamming
            mensagemComControle = CamadaEnlaceDadosTransmissoraControleDeErroCodigoDeHamming(quadro);
    }

    return mensagemComControle;
}

string CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(string mensagem) {

    string contagem;            //Bit de paridade
    
    int contador = 0;       // Contagem do numeors de 1s na mensagem
    for (char c : mensagem) {
        if (c == '1') {
            contador++;
        }
    }


    if (contador % 2 == 0) { contagem = '0';} //Vendo se o numero de 1s e par ou impar
    else { contagem = '1'; }

    mensagem += contagem; // Somando valor do bit pardiade ao da mensagem

    return mensagem; 
    
}



string CamadaEnlaceDadosTransmissoraControleDeErroCRC(string mensagem) {

    string cacheMensagemRecebida = mensagem;

    for (long unsigned int i=0; i < polinomioGerador.size()-1; i++) {   //Adicionando o numero de 0s do polinomioGerador - 1 para divisao
        mensagem = mensagem + "0";
    }    

    string CRC = mensagem.substr(0, polinomioGerador.size());   //Variavel que ira armazenar o resto das divisoes sucessivas
    unsigned long polinomioInt = bitset<64>(polinomioGerador).to_ulong();   //Valor do polinomioGerador em unigned long para fazer XOR

    long unsigned int index = CRC.size();   //Index para saber qual numero desce na próxima divisão

    //While que fará as divisões sucessivas
    while (true) {
        if (index > mensagem.size()) {  //Saber chegou no ultimo numero a ser dividido
            break;
        }

        unsigned long CRCint = bitset<64>(CRC).to_ulong();  //Convertendo CRC de string para unsigned long para fazer XOR

        if (CRC.size() == polinomioGerador.size()) {            //Só faz divisão se for possivel (Colocar 0 no quociente e descer um numero)
            unsigned long resultXor = CRCint^polinomioInt;
            CRC = decimalToBinary(resultXor);
        }

        CRC = CRC + mensagem.substr(index, 1); //Desce o proximo numero para divisão

        index += 1;

    }

    // While para deixar o CRC com o tamanho padronizado para ser enviado
    while (true) {
        if (CRC.size() < polinomioGerador.size() - 1) {
            CRC = "0" + CRC;
        } else {
            break;
        }
    }

    mensagem = cacheMensagemRecebida + CRC;
    
    return mensagem;
}

string CamadaEnlaceDadosTransmissoraControleDeErroCodigoDeHamming(string mensagem) {
    string data = mensagem;
    
    // Calcula o número de bits de paridade a serem adicionados
    int numParityBits = 0;
    while ((int)data.size() + numParityBits + 1 > (1 << numParityBits))
        numParityBits++;

    // Cria uma string temporária para os dados e bits de paridade
    std::string encodedData;
    encodedData.reserve(data.size() + numParityBits);

    // Posições iniciais dos bits de paridade
    int parityPos = 1;
    int dataIndex = 0;

    // Percorre a string final e insere os bits de paridade nas posições corretas
    for (long unsigned int i = 1; i <= data.size() + numParityBits; i++) {
        if (i == parityPos) {
            // Insere um espaço em branco para o bit de paridade
            encodedData.push_back(' ');
            parityPos <<= 1; // Próxima posição de bit de paridade
        } else {
            // Insere um bit de dados
            encodedData.push_back(data[dataIndex]);
            dataIndex++;
        }
    }

    //Achando as posições com 1s
    std::vector<int> onesPositions;
    for (long unsigned int i = 0; i < encodedData.size(); i++) {
        if (encodedData[i] == '1') {
            onesPositions.push_back(i+1);
        }
    }

    //Fazendo XORs para obter os numeros dos bits redundantes
    int result=-1;
    for (long unsigned int i=0;i<onesPositions.size(); i++) { 
        if (result == -1) {
            result = onesPositions[i];
            continue;
        }
        result = result^onesPositions[i];
    } 

    string bitsParidade = decimalToBinary(result);
    while(true) {
        if ((int)bitsParidade.size() < numParityBits) {
            bitsParidade = "0" + bitsParidade;
        } else {
            break;
        }
    }

    int index = bitsParidade.size() - 1;
    for (size_t i=0; i<encodedData.size(); ++i) {
        if (encodedData[i] == ' '){
            encodedData[i] = bitsParidade[index];
            index -= 1;
        }
    }

    return encodedData;
    
}

string CamadaEnlaceTransmissoraEnquadramento(string quadro) {
    string quadroEnquadrado;

    int EnqType;
    cout << "Escolha o tipo de enquadramento" << endl;
    cout << "0 - Contagem de caracter" << endl;
    cout << "1 - Inserção de Bytes" << endl;
    scanf("%d", &EnqType);
    tipoDeEnquadramento = EnqType;

    switch (tipoDeEnquadramento) {
        case 0: // Contagem de caracteres
            quadroEnquadrado = CamadaEnlaceTransmissoraEnquadramentoContagemDeCaracteres(quadro);
            break;

        case 1: // Insercao de bytes
            quadroEnquadrado = CamadaEnlaceTransmissoraEnquadramentoInsercaoDeBytes(quadro);
            break;
    }

    return quadroEnquadrado;
}

string CamadaEnlaceTransmissoraEnquadramentoContagemDeCaracteres(string mensagem) {
    int MAX = tamanhoCabecalhoContagemDeCaracter;
    int nMensagens = mensagem.size() / MAX;  //Divisao em n quadros
    if (mensagem.size() % MAX != 0) { nMensagens += 1;}

    string SendMensagem [nMensagens];
    
    for (int i=0;i<nMensagens;i++) {
        SendMensagem[i] = mensagem.substr(MAX*i, MAX);      //Separando a Mensagem em n quadros de MAX caracteres
        
        string tamanhoBinario;      //Numero de bits na palavra em string
        int numero = SendMensagem[i].size();
        tamanhoBinario = decimalToBinary(numero);

        //Fazendo com que o tamanho d0 cabecalho ocupe 1 byte (8 bits)
         while (tamanhoBinario.size() < 8) {
            tamanhoBinario = "0" + tamanhoBinario;
        }


        SendMensagem[i] = tamanhoBinario + SendMensagem[i];    //juncao do cabecalho (tamanhoFinal) com a mensagem
    }
    
    string MensagemFinal; //Juntando quadros para fazer envio
    for (int i=0; i<nMensagens; i++) {
        MensagemFinal = MensagemFinal + SendMensagem[i];
    }

    return MensagemFinal;

    
    
}

string CamadaEnlaceTransmissoraEnquadramentoInsercaoDeBytes(string mensagem) {
    int nMensagens = mensagem.size() / espacoInsercaoBytes; //Divisão da mensagem em n quadris
    if (mensagem.size() % espacoInsercaoBytes != 0) { nMensagens += 1;} //Ajuste do tamanho de quadros

    string SendMensagem [nMensagens];

    string MensagemFinal;

    for (int i=0;i<nMensagens;i++) {
        SendMensagem[i] = mensagem.substr(espacoInsercaoBytes*i, espacoInsercaoBytes);      //Separando a Mensagem em n quadros de MAX caracteres
        
        for (int j=0; j<2 ; j++) {
            string value;
            if (j == 0) {value = escape;}
            else if (j == 1) {value = flag;}
            else {cout<<"erro(1): "<< j; retransmitir();}
        
        size_t pos = SendMensagem[i].find(value); //Verificando se tem flag na menagem

        int control = 0;
        while (pos != std::string::npos) {
            if (pos != std::string::npos) {
                string cacheMensagem1 = SendMensagem[i].substr(0, pos);
                string cacheMensagem2 = SendMensagem[i].substr(pos+8);
                SendMensagem[i] = cacheMensagem1 + escape + value + cacheMensagem2;

                pos = SendMensagem[i].find(value, pos + 9);

                control += 1; if (control > 10) { cout<< "erro(2)" << endl; retransmitir();}
            }
        }
        }
        SendMensagem[i] = flag + SendMensagem[i] + flag;
        MensagemFinal = MensagemFinal + SendMensagem[i];
    }
    return MensagemFinal;
}

//******************************************************************RECEPECAO***************************************************
string CamadaEnlaceDadosReceptoraEnquadramentoContagemDeCaracteres(string mensagem) {
    
    string MensagemRecebida;
    
    int tamanhoTotal = mensagem.size();
    int index = 0;
    int totalCaracter = 0;

    while (tamanhoTotal > index) {
        int cabecalho = std::stoi(mensagem.substr(index,8));
        cabecalho = binarioParaDecimal(cabecalho);
        totalCaracter += (cabecalho+8);
        index += 8;

        if (index + cabecalho > (int)mensagem.length()) { //Verificando erros
            cout << "Erro na contagem de caracter: tentando ler mensagem inexistente" << endl;
            retransmitir();
            break;
        }
        string quadro = mensagem.substr(index, cabecalho);
        MensagemRecebida = MensagemRecebida + quadro;

        index += quadro.size();
    } 

    if (totalCaracter != (int)mensagem.length()) { //Verificando se toda mensagem foi lida
        cout << "Erro de contagem de caracter: Não foi lido todos o caracteres" << endl;
        retransmitir();
    }

    return MensagemRecebida;

    
}

string CamadaEnlaceDadosReceptoraEnquadramentoInsercaoDeBytes(string mensagem) {
    if (mensagem.substr(0, 8) != flag) {
        cout << "erro na primeira flag" << endl;
        retransmitir();
    }


    vector<int> posicoes;
    posicoes.push_back(0);

    size_t pos = mensagem.find(flag);
    int flagNumbers = 1;
    while (pos != std::string::npos) {
        if (pos != 0) { 

            //verificando numero de flags na mensagem
            int check = 0;
            int escapeNumbers =  0;
            int relPos = pos;
            while (check == 0) {
                if (mensagem.substr(relPos-8,8) == escape) {
                    escapeNumbers += 1;
                    relPos += -8;
                    if  (relPos < 0) { cout << "killl"; retransmitir();}
                } else {
                    check = 1;
                }
            }
            if (escapeNumbers % 2 == 0) { //flag real
                flagNumbers += 1;
                int cache = pos;
                posicoes.push_back(cache);
            }
        }
        pos = mensagem.find(flag, pos + 1);
    }
    if (flagNumbers % 2 != 0) { cout << "Erro detectado (flagError)" << endl; retransmitir();}

    int tamanho_nMensagens = flagNumbers / 2;
    string subMensagens[tamanho_nMensagens];

    for (int i=0;i < tamanho_nMensagens; i++) { //Separando em submensagens (quadros)
        int j = i*2;
        subMensagens[i] = mensagem.substr(posicoes[j], posicoes[j+1] - posicoes[j] + 8);
    }
    
    string MensagemDesenquadrada;
    for (int i=0; i<tamanho_nMensagens; i++) {
        string cachMsg = subMensagens[i]; //cahce de mensagem para armezanar o quadro de carga util (sem flags, escapes...)

        if (cachMsg.substr(0,8) != flag) {//Verificando erro no inicio do quadro
            cout << "Erro na primeira flag do quadro " << i << endl;
            retransmitir();
        }
        if (cachMsg.substr(cachMsg.size()-8, 8) != flag) { //Verificadno erro no final do quadro
            cout << "Erro na ultima flag do quadro " << i << endl;
            retransmitir();
        }

        cachMsg = cachMsg.substr(8); //Retirando flag de incio
        cachMsg = cachMsg.substr(0, cachMsg.size() - 8); //Retirando flag de final

        size_t escapePos = cachMsg.find(escape);

        while (escapePos != std::string::npos) { //retirando escapes
            
            cachMsg = cachMsg.substr(0,escapePos) + cachMsg.substr(escapePos+8, cachMsg.size() - escapePos);
            escapePos = cachMsg.find(escape, escapePos + 1);
        }
        MensagemDesenquadrada = MensagemDesenquadrada + cachMsg;
    }
    
    return MensagemDesenquadrada;
}

string CamadaEnlaceDadosReceptoraEnquadramento (string quadro) {
    string quadroDesenquadrado;

    switch (tipoDeEnquadramento) {
        case 0: //Contagem de caracteres
            quadroDesenquadrado = CamadaEnlaceDadosReceptoraEnquadramentoContagemDeCaracteres(quadro);
            break;
        case 1: //Insercao de bytes
            quadroDesenquadrado = CamadaEnlaceDadosReceptoraEnquadramentoInsercaoDeBytes(quadro);
            break;
    }
    return quadroDesenquadrado;
}






string CamadaEnlaceDadosReceptoraControleDeErroCodigoDeHamming(string mensagem) {
    string encodedData = mensagem;
    //Achando as posições com 1s
    std::vector<int> onesPositions;
    for (long unsigned int i = 0; i < encodedData.size(); i++) {
        if (encodedData[i] == '1') {
            onesPositions.push_back(i+1);
        }
    }

    //Fazendo XORs para obter os numeros dos bits redundantes
    int result=-1;
    for (long unsigned int i=0;i<onesPositions.size(); i++) { 
        if (result == -1) {
            result = onesPositions[i];
            continue;
        }
        result = result^onesPositions[i];
    }

    if (result != 0) {
        cout << "Erro detectado no código de hamming" << endl;
        retransmitir();
    }
    
    string mensagemSemControle;

    for (size_t i = 0; i < mensagem.length(); ++i) {
        if (!((i+1) & (i)) == 0) {
            mensagemSemControle += mensagem[i];
        }
    }
    
    return mensagemSemControle;
}


string CamadaEnlaceDadosReceptoraControleDeErroCRC(string mensagem) {
    
    string CRC = mensagem.substr(0, polinomioGerador.size());       
    unsigned long polinomioInt = bitset<64>(polinomioGerador).to_ulong();

    long unsigned int index = CRC.size();
    
    //while que irá fazer as diviões sucessivas
    while (true) {
        if (index > mensagem.size()) {
            break;
        }

        unsigned long CRCint = bitset<64>(CRC).to_ulong();

        if (CRC.size() == polinomioGerador.size()) {
            unsigned long resultXor = CRCint^polinomioInt;
            CRC = decimalToBinary(resultXor);
        }

        CRC = CRC + mensagem.substr(index, 1);

        index += 1;
    }

    
    unsigned long CRCint = bitset<64>(CRC).to_ulong();
    
    if(CRCint != 0) {           // Verificando se o resto da divisão é igual a 0
        cout << "Erro detectado pelo CRC" << endl;
        retransmitir();
    } 

    for (long unsigned int i=0; i<polinomioGerador.size() - 1; i++){
        mensagem.pop_back(); // Retirando os controles geradios pelo CRC para leitura correta da mensagem
    }
    return mensagem;
}

string CamadaEnlaceDadosReceptoraControleDeErroBitDeParidadePar(string mensagem) {
    int contador = 0;
    for (char c : mensagem) {   //Contando numeors de 1s na mensagem
        if (c == '1') {
            contador++;
        }
    }

    if (contador % 2 != 0) { //Vendo se a quantidade de 1s é par
        cout << "Erro na paridade" << endl;
        retransmitir();
    }


    mensagem.pop_back();
    return mensagem;
}

string CamadaEnlaceReceptoraControleDeErro (string quadro) {
    switch(tipoDeControleDeErro) {
        case 0: //Bit paridade
            quadro = CamadaEnlaceDadosReceptoraControleDeErroBitDeParidadePar(quadro);
            break;
        case 1: //CRC
            quadro = CamadaEnlaceDadosReceptoraControleDeErroCRC(quadro);
            break;
        case 2: //Hamming
            quadro = CamadaEnlaceDadosReceptoraControleDeErroCodigoDeHamming(quadro);
    }

    return quadro;
}

void CamadaEnlaceDadosReceptora(string quadro) {
    string MensagemRecebida = quadro;
    
    MensagemRecebida = CamadaEnlaceReceptoraControleDeErro(MensagemRecebida);
    MensagemRecebida = CamadaEnlaceDadosReceptoraEnquadramento(MensagemRecebida);
    
    CamadaDeAplicacaoReceptora(MensagemRecebida);
}
