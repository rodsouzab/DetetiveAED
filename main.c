#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Definições de cores ANSI
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define RESET "\x1B[0m"

void printColor(const char* color, const char* text) {
    printf("%s%s\033[0m", color, text); // Reseta a cor após a impressão
}

typedef struct Carta {
    int id;
    char nome[30];
} Carta;

typedef struct Espaco {
    int id;
    struct Espaco *ant;
    struct Espaco *prox;
    char local[35];
} Espaco;

typedef struct Jogador {
    int id;
    struct Jogador *prox;
    Espaco *espaco;
    Carta cartas[5];
    Espaco *ultimoPalpite;
} Jogador;

void loopJogo();
Espaco *iniciarTabuleiro();
void *iniciarJogadores(Jogador **head, Jogador **tail, Espaco *espaco);
void inserirEspaco(Espaco **espaco, int id);
void imprimirTabuleiroByLocal(Espaco *head);
void inserirJogador(Jogador **head, Jogador **tail, int id, Espaco *espaco);
void imprimirJogadoresbyLocal(Jogador *head, Jogador *tail);
void iniciarCartas(Jogador *jogadorHead, Jogador *jogadorTail, Carta final[3]);
void bubbleSort(Jogador *jogadorHead, Jogador *jogadorTail);
void mostrarMenuPrevio();
void clearScreen();
void lerRegras();
int jogarDado();
int moverJogador(Jogador *jogador, int sentido, int qtdMovimentos);
Espaco *buscaEspaco(Espaco *espaco, int id);
void mostrarMenu(Jogador *jogadorAtual, Espaco *tabuleiroHead);
void imprimirCartasJogador(Jogador *jogador);
void mostrarMenuDeMovimento(Jogador *jogadorAtual, Espaco *tabuleiroHead, int qtdMovimentos);
void printAssassinos();
void printArmas();
void printLocais();
int idPalpiteLocal(Espaco *espaco);
void conferirPalpite(Jogador *jogadorAtual, int idAssassino, int idArma, int idLocal);
int conferirPalpiteFinal(Jogador *jogadorAtual, int idAssassino, int idArma, int idLocal, Carta final[]);
void removerJogador(Jogador **jogadorHead, Jogador **jogadorTail, int id);
void printCartas();

int main() {
    srand(time(NULL));
    clearScreen();

    int comando = 0;

    while (1) {
        mostrarMenuPrevio();
        scanf("%d", &comando);
        switch (comando) {
            case 1:
                loopJogo();
                break;
            case 2:
                lerRegras();
                clearScreen();
                break;
            case 3:
                exit(0);
                break;
            default:
                clearScreen();
                printf("\n\nComando Inválido, tente novamente.\n\n");
                break;
        }
    }


}

void loopJogo() {
    Espaco *tabuleiroHead = iniciarTabuleiro();
    Espaco *espacoInicial = buscaEspaco(tabuleiroHead, 18);
    Jogador *jogadorHead = NULL;
    Jogador *jogadorTail = NULL;
    iniciarJogadores(&jogadorHead,&jogadorTail,espacoInicial);
    Carta final[3];
    iniciarCartas(jogadorHead, jogadorTail, final);

    //printf("%s %s %s", final[0].nome, final[1].nome, final[2].nome);
    //sleep(4);

    clearScreen();

    printf("Iniciando Jogo...\n");
    sleep(4);

    clearScreen();

    int comando = 0;
    Jogador *jogadorAtual = jogadorHead;
    int turnoConfirmed = 0;
    int fimJogo = 0;
    int idGanhador = 0;

    while(1) {
        comando = 0;
        printf("Turno do Jogador %d\n", jogadorAtual->id);
        //printf("Head: %d\nTail: %d\n", jogadorHead->id, jogadorTail->id);
        //printf("%s %s %s\n", final[0].nome, final[1].nome, final[2].nome);
        while (comando != 1) {
            printf("\nDigite '1' para começar:");
            printf("\n");
            scanf("%d", &comando);
        }
        clearScreen();

        mostrarMenu(jogadorAtual, tabuleiroHead);

        scanf("%d",&comando);
        clearScreen();
        int resultadoDado; 
        switch (comando) {
            case 1:
                resultadoDado = jogarDado();
                printf("\n\nO resultado do dado foi %d", resultadoDado);
                comando = 0;
                int qtdMovimentos = resultadoDado;
                while (comando != 1) {
                    printf("\n\nDigite '1' para se locomover:");
                    printf("\n");
                    scanf("%d", &comando);
                }
                clearScreen();
                while (qtdMovimentos > 0) {
                    mostrarMenuDeMovimento(jogadorAtual,tabuleiroHead,qtdMovimentos);
                    scanf("%d",&comando);
                    clearScreen();
                    qtdMovimentos = moverJogador(jogadorAtual,comando,qtdMovimentos);
                }
                turnoConfirmed = 1;
                break;
            case 2:
                if (strcmp(jogadorAtual->espaco->local, "/0") != 0 && jogadorAtual->ultimoPalpite != jogadorAtual->espaco) {
                    int palpiteAssassino, palpiteArma, palpiteLocal;
                    printf(RED"Faça seu Palpite.\n\n"RESET);
                    printAssassinos();
                    printf("\nDigite o assassino que você quer palpitar:\n");
                    scanf("%d",&palpiteAssassino);
                    palpiteAssassino -= 1; //Correção de id
                    clearScreen();

                    printf(RED"Faça seu Palpite.\n\n"RESET);
                    printArmas();
                    printf("\nDigite a arma que você quer palpitar:\n");
                    scanf("%d",&palpiteArma);
                    palpiteArma += 7; //Correção de id
                    clearScreen();

                    palpiteLocal = idPalpiteLocal(jogadorAtual->espaco);

                    conferirPalpite(jogadorAtual, palpiteAssassino, palpiteArma, palpiteLocal);

                    jogadorAtual->ultimoPalpite = jogadorAtual->espaco;

                    turnoConfirmed = 1;
                } else turnoConfirmed = 0;
                break;
            case 3:
                if (jogadorAtual->espaco->id == 0) {
                    Jogador *aux = jogadorAtual;
                    while (aux->espaco->prox != NULL)
                        aux->espaco = aux->espaco->prox;
                    jogadorAtual->espaco = aux->espaco;
                    turnoConfirmed = 1;
                } else {
                    while (comando != 1) {
                    printf("\nComando Inválido. Digite '1' para voltar ao menu:");
                    printf("\n");
                    scanf("%d", &comando);
                    }
                    turnoConfirmed = 0;
                }
                break;
            case 4:
                if (jogadorAtual->espaco->id == 34) {
                    Jogador *aux = jogadorAtual;
                    while (aux->espaco->ant != NULL)
                        aux->espaco = aux->espaco->ant;
                    jogadorAtual->espaco = aux->espaco;
                    turnoConfirmed = 1;
                } else {
                    while (comando != 1) {
                    printf("\nComando Inválido. Digite '1' para voltar ao menu:");
                    printf("\n");
                    scanf("%d", &comando);
                    }
                    turnoConfirmed = 0;
                }
                break;
            case 5:
                if (jogadorAtual->espaco->id == 18) {
                    int palpiteAssassino, palpiteArma, palpiteLocal;
                    printf(RED"Faça seu Palpite final.\n\n"RESET);
                    printAssassinos();
                    printf("\nDigite o assassino que você quer palpitar:\n");
                    scanf("%d",&palpiteAssassino);
                    palpiteAssassino -= 1; //Correção de id
                    clearScreen();

                    printf(RED"Faça seu Palpite Final.\n\n"RESET);
                    printArmas();
                    printf("\nDigite a arma que você quer palpitar:\n");
                    scanf("%d",&palpiteArma);
                    palpiteArma += 7; //Correção de id
                    clearScreen();

                    printf(RED"Faça seu Palpite Final.\n\n"RESET);
                    printLocais();
                    printf("\nDigite o local que você quer palpitar:\n");
                    scanf("%d",&palpiteLocal);
                    palpiteLocal += 14; //Correção de id
                    clearScreen();

                    fimJogo = conferirPalpiteFinal(jogadorAtual, palpiteAssassino, palpiteArma, palpiteLocal, final);

                    if (fimJogo == 0) {
                        removerJogador(&jogadorHead, &jogadorTail, jogadorAtual->id);
                        printf(RED"JOGADOR %d FOI ELIMINADO!\n\n"RESET, jogadorAtual->id);
                        printf(RED"SUAS CARTAS ERAM:\n"RESET);
                        imprimirCartasJogador(jogadorAtual);
                        comando = 0;
                        while (comando != 1) {
                            printf("\nDigite '1' para continuar:");
                            printf("\n");
                            scanf("%d", &comando);
                        }
                        clearScreen();
                    }

                    if (fimJogo == 1)
                        idGanhador = jogadorAtual->id;
                        
                    if (jogadorHead->id == jogadorTail->id) {
                        fimJogo = 1;
                        idGanhador = jogadorHead->id;
                    }
                    
                    turnoConfirmed = 1;
                } else turnoConfirmed = 0;
                break;
            case 6: 
                printCartas();
                turnoConfirmed = 0;
                break;
            default:
                while (comando != 1) {
                    printf("\nComando Inválido. Digite '1' para voltar ao menu:");
                    printf("\n");
                    scanf("%d", &comando);
                }
                turnoConfirmed = 0;
                break;
            }
        
        if (fimJogo ==  1) {
            printf(YELLOW"FIM DE JOGO.\n\nGANHADOR: JOGADOR %d"RESET, idGanhador);
            printf(YELLOW"\n\nO Crime foi cometido pelo %s, com um(a) %s no(a) %s\n\n"RESET, final[0].nome, final[1].nome, final[2].nome);
            comando = 0;
            while (comando != 1) {
                printf("\nDigite '1' para continuar:");
                printf("\n");
                scanf("%d", &comando);
            }
            clearScreen();
            break;
        }
        
        if (turnoConfirmed == 1) {
            jogadorAtual = jogadorAtual->prox;
            turnoConfirmed = 0;
        }
    }
}

void removerJogador(Jogador **jogadorHead, Jogador **jogadorTail, int id) {
    if (*jogadorHead != NULL) {
        Jogador *aux = *jogadorHead;
        while (aux->id != id)
            aux = aux->prox;
        if (aux == *jogadorHead) {
            *jogadorHead = (*jogadorHead)->prox;
            (*jogadorTail)->prox = *jogadorHead;
        } else {
            Jogador *aux2 = *jogadorHead;
            while (aux2->prox != aux)
                aux2 = aux2->prox;
            aux2->prox = aux->prox;
            if (aux2->prox == aux2) {
                *jogadorHead = *jogadorTail;
            }
        }
    }
}

int conferirPalpiteFinal(Jogador *jogadorAtual, int idAssassino, int idArma, int idLocal, Carta final[]) {
    Jogador *aux = jogadorAtual->prox;  
    int c = 0;  


    for (int i = 0; i < 3; i++) {
        if (final[i].id == idAssassino || final[i].id == idArma || final[i].id == idLocal) {
            c++;
        }
    }

    clearScreen();

    printf("Conferindo palpite final...");
    sleep(7);

    clearScreen();

    if (c == 3) 
        return 1;
    else 
        return 0;
    
}

void conferirPalpite(Jogador *jogadorAtual, int idAssassino, int idArma, int idLocal) {
    Jogador *aux = jogadorAtual->prox;  
    Carta palpitesEncontrados[3];  
    int c = 0;  

    while (aux != jogadorAtual) {
        for (int i = 0; i < 5; i++) {
            if (aux->cartas[i].id == idAssassino || aux->cartas[i].id == idArma || aux->cartas[i].id == idLocal) {
                palpitesEncontrados[c] = aux->cartas[i];
                c++;
                if (c == 3) {
                    break;
                }
            }
        }
        if (c > 0) {
            break;
        }
        aux = aux->prox;  
    }

    clearScreen();

    printf("Conferindo palpite...");
    sleep(2);

    clearScreen();

    if (c == 0) {
        printf("Nenhum palpite foi confirmado.\n");
        int comando = 0;
        while (comando != 1) {
                    printf("\nDigite '1' para continuar\n");
                    printf("\n");
                    scanf("%d", &comando);
                }
        clearScreen();
        return;
    } else {
        printf(RED"O palpite foi encontrado pelo jogador %d\n\n"RESET,aux->id);
        int comando = 0;
        while (comando != 1) {
                    printf("(Jogador %d) Digite '1' para selecionar a carta que vai ser mostrada ao Jogador %d\n",(aux->id), (jogadorAtual->id));
                    printf("\n");
                    scanf("%d", &comando);
                }
        clearScreen();
        

        for (int i = 0; i < c; i++) 
            printf(GREEN"%d. %s"RESET, i + 1, palpitesEncontrados[i].nome);
            

        int escolha = 0;
        while (escolha < 1 || escolha > c) {
            printf("\n\nSelecione que carta mostrar ao Jogador %d:\n",(jogadorAtual->id));
            scanf("%d",&escolha);
        }
        clearScreen();

        comando = 0;
        while (comando != 1) {
            printf(" (Jogador %d) Carta selecionada. Digite '1' para vizualizá-la:\n", (jogadorAtual->id));
            scanf("%d",&comando);
        }
        clearScreen();

        printf("Carta Selecionada: "GREEN"%s\n"RESET,palpitesEncontrados[escolha-1].nome);

        comando = 0;
        while (comando != 1) {
            printf("\nDigite '1' para encerrar o turno: \n");
            scanf("%d",&comando);
        }
        clearScreen();
    }
}


void mostrarMenuDeMovimento(Jogador *jogadorAtual, Espaco *tabuleiroHead, int qtdMovimentos) {
    printf("Tabuleiro:");
    imprimirTabuleiroByLocal(tabuleiroHead);

    if (strcmp(jogadorAtual->espaco->local,"\0") != 0)
        printf("\nVocê está no(a) %s\n",jogadorAtual->espaco->local);
    else
        printf("\nVocê está no espaço %d\n",jogadorAtual->espaco->id);

    printf("\nMovimentos restantes: %d\n", qtdMovimentos);

    printf("\nDigite o que você deseja fazer: \n");
    printf("1. Mover para a esquerda\n");
    printf("2. Mover para a direita\n");
    printf("3. Parar\n");
}

void mostrarMenu(Jogador *jogadorAtual, Espaco *tabuleiroHead) {
    printf("Tabuleiro:");
    imprimirTabuleiroByLocal(tabuleiroHead);

    printf("\nSuas Cartas:\n");
    imprimirCartasJogador(jogadorAtual);

    if (strcmp(jogadorAtual->espaco->local,"\0") != 0)
        printf("Você está no(a) %s\n",jogadorAtual->espaco->local);
    else
        printf("Você está no espaço %d\n",jogadorAtual->espaco->id);

    printf("\nDigite o que você deseja fazer: \n");
    printf("1. Jogar Dado\n");
    if (strcmp(jogadorAtual->espaco->local,"\0") != 0 && jogadorAtual->ultimoPalpite != jogadorAtual->espaco)
        printf("2. Fazer Palpite\n");
    if (jogadorAtual->espaco->id == 0)
        printf("3. Ir para Ponto de Ônibus\n");
    if (jogadorAtual->espaco->id == 34)
        printf("4. Ir para Metrô\n");
    if (jogadorAtual->espaco->id == 18)
        printf("5. Fazer Palpite Final\n");
    printf("6. Ver Todas as Cartas\n");

}

Espaco *buscaEspaco(Espaco *espaco, int id) {
    if (espaco != NULL) {
        while(espaco->id != id)
            espaco = espaco->prox;
        return espaco;
    }
    return NULL;
}

void iniciarCartas(Jogador *jogadorHead, Jogador *jogadorTail, Carta final[3]) {
    FILE* arquivo = fopen("cartas.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de cartas.\n");
        return;
    }

    char buffer[100];
    int id = 0;
    Carta cartas[23];

    while (fgets(buffer, 100, arquivo) != NULL && id < 23) {
        buffer[strcspn(buffer, "\n")] = 0; 
        cartas[id].id = id;
        strcpy(cartas[id].nome, buffer);
        id++;
    }
    fclose(arquivo);

    int r1 = rand() % 8; // Assassino
    final[0] = cartas[r1];
    strcpy(cartas[r1].nome, "");

    int r2 = rand() % 7 + 8; // Arma
    final[1] = cartas[r2];
    strcpy(cartas[r2].nome, "");

    int r3 = (rand() % 8) + 15; // Local
    final[2] = cartas[r3];
    strcpy(cartas[r3].nome, "");




    Jogador *aux = jogadorHead;

    do {
        int countDeck = 0;
        while (countDeck < 5) {
            int random = rand() % 23;

            if (strlen(cartas[random].nome) != 0) {
                aux->cartas[countDeck] = cartas[random];
                strcpy(cartas[random].nome, "");
                countDeck++;
            }
        }
        aux = aux->prox; // Avançar para o próximo jogador usando o ponteiro auxiliar
    } while (aux != jogadorTail->prox);

    aux = jogadorHead;

    bubbleSort(aux, jogadorTail);
}

void bubbleSort(Jogador *jogadorHead, Jogador *jogadorTail) {
    do {
        for (int n = 1; n <= 5; n++) {
            for (int j = 0; j < 4; j++) {
                if (jogadorHead->cartas[j].id > jogadorHead->cartas[j+1].id) {
                    Carta aux = jogadorHead->cartas[j];
                    jogadorHead->cartas[j] = jogadorHead->cartas[j+1];
                    jogadorHead->cartas[j+1] = aux;
                }
            }
        }
        jogadorHead = jogadorHead->prox;
    } while (jogadorHead != jogadorTail->prox);
}

void *iniciarJogadores(Jogador **head, Jogador **tail, Espaco *espaco) {
    for (int i = 4; i >= 1; i--) {
        inserirJogador(&(*head), &(*tail), i, espaco);
    }
}

Espaco *iniciarTabuleiro() {
    Espaco *espaco = NULL;
    for (int i = 34; i >= 0; i--) {
        inserirEspaco(&espaco, i);
        switch (espaco->id){
            case 0:
                strcpy(espaco->local, "Metrô");
                break;
            case 7:
                strcpy(espaco->local, "Mansão");
                break;
            case 12:
                strcpy(espaco->local, "Prefeitura");
                break;
            case 18:
                strcpy(espaco->local, "Parque");
                break;
            case 22:
                strcpy(espaco->local, "Praia");
                break;
            case 25:
                strcpy(espaco->local, "Escola");
                break;
            case 29:
                strcpy(espaco->local, "Ponte");
                break;
            case 34:
                strcpy(espaco->local, "Ponto de Ônibus");
                break;
            default:
                strcpy(espaco->local, "\0");
                break;
        }
    }
    return espaco;
}

void inserirJogador(Jogador **head, Jogador **tail, int id, Espaco *espaco) {
  Jogador *novo = (Jogador *) malloc(sizeof(Jogador));
  if (novo != NULL) {
    novo->id = id;
    novo->espaco = espaco;
    novo->ultimoPalpite = espaco;
    if (*head == NULL) {
      *head = *tail = novo;
    } else {
      novo->prox = *head;
      *head = novo;
    }
    (*tail)->prox = *head;
  }
}

void inserirEspaco(Espaco **espaco, int id) {
    Espaco *novo = (Espaco*)malloc(sizeof(Espaco));
    if (novo != NULL) {
        novo->id = id;
        novo->prox = *espaco;
        novo->ant = NULL;

        if (*espaco != NULL) {
            (*espaco)->ant = novo;
        }
        *espaco = novo;
    }
}

void imprimirJogadoresbyLocal(Jogador *head, Jogador *tail) {
    Jogador *current = head;
    printf("\nJogadores no jogo:\n");
    printf("ID\tPosição\t\tCartas\n");
    do {
        printf("%d\t%s\t\t", current->id, current->espaco->local);
        for (int i = 0; i < 5; i++) {
            printColor(GREEN, current->cartas[i].nome);
            printf("\t\t\t");
        }
        printf("\n");
        current = current->prox;
    } while (current != tail->prox);
    printf("\n");
}

void imprimirTabuleiroByLocal(Espaco *head) {
    Espaco *current = head;
    printf(CYAN "\n---------------------------------------------\n" RESET);
    while (current != NULL) {
        if (strcmp(current->local,"\0") != 0)
            printf(YELLOW "%s" RESET, current->local);
        else
            printf("%d", current->id);
        current = current->prox;
        if (current != NULL) {
            printf(" <=> ");
        }
    }
    printf(CYAN "\n---------------------------------------------\n" RESET);
}

void mostrarMenuPrevio() {
    printf(GREEN "\n=============================\n" RESET);
    printf(MAGENTA "        DETETIVE 🔍\n" RESET);
    printf(GREEN "=============================\n" RESET);
    printf("1. 🕵️ Iniciar Jogo\n");
    printf("2. 📜 Ver Regras\n");
    printf("3. 🚪 Sair\n");
    printf(GREEN "=============================\n" RESET);
    printf("Escolha uma opção: ");
}

void clearScreen() {
    printf("\033[2J\033[H"); // Limpa a tela e move o cursor para o topo
}

void lerRegras() {
    FILE *arquivo = fopen("regras.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de regras.\n");
        return;
    }

    clearScreen();

    char buffer[500];
    while (fgets(buffer, 500, arquivo) != NULL) {
        printf("%s", buffer);
    }

    printf("\n");
    int c = 0;

    while (c != 1) {
        printf("\nDigite '1' para continuar:");
        printf("\n");
        scanf("%d", &c);
    }
    fclose(arquivo);
}

int jogarDado() {
    printf("Jogando o dado...");
    sleep(2);
    
    return rand() % 6 + 1; // Gera um número entre 1 e 6

    //return 20;
}

int moverJogador(Jogador *jogador, int sentido, int qtdMovimentos) {
    if (sentido == 1) {
        if (jogador->espaco->id != 0) {
            jogador->espaco = jogador->espaco->ant;
            qtdMovimentos--;
        }
    }
    if (sentido == 2) {
        if (jogador->espaco->id != 34) {
            jogador->espaco = jogador->espaco->prox;
            qtdMovimentos--;
        }
    }
    if (sentido == 3) {
        qtdMovimentos = 0;
    }
    return qtdMovimentos;
}

void imprimirCartasJogador(Jogador *jogador) {
    for (int i = 0; i < 5; i++) {
            printColor(GREEN, jogador->cartas[i].nome);
            printf("\n");
        }
        printf("\n");
};

void printAssassinos() {
    FILE* arquivo = fopen("cartas.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de cartas.\n");
        return;
    }

    char buffer[100];
    int id = 0;
    int c = 1;
    Carta cartas[20];

    while (fgets(buffer, 100, arquivo) != NULL && id < 8) {
        printf(GREEN"%d. %s"RESET,c,buffer);
        id++; c++;
    }
    fclose(arquivo);
}

void printArmas() {
    FILE* arquivo = fopen("cartas.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de cartas.\n");
        return;
    }

    char buffer[100];
    int id = 0;
    int c = 1;
    Carta cartas[20];

    while (fgets(buffer, 100, arquivo) != NULL && id < 15) {
        if (id > 7) {
            printf(GREEN"%d. %s"RESET,c,buffer);
            c++;
        }
        id++; 
    }
    fclose(arquivo);
}

void printLocais() {
    FILE* arquivo = fopen("cartas.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de cartas.\n");
        return;
    }

    char buffer[100];
    int id = 0;
    int c = 1;
    Carta cartas[20];

    while (fgets(buffer, 100, arquivo) != NULL && id < 23) {
        if (id > 14) {
            printf(GREEN"%d. %s"RESET,c,buffer);
            c++;
        }
        id++; 
    }
    fclose(arquivo);
}

int idPalpiteLocal(Espaco *espaco) {
    FILE* arquivo = fopen("cartas.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de cartas.\n");
        exit(0);
    }

    char buffer[100];
    int id = 0;

    while (fgets(buffer, 100, arquivo) != NULL && id < 23) {
        if (id > 15) {
             buffer[strcspn(buffer, "\n")] = 0;
            
            if (strcmp(espaco->local,buffer) == 0) {
                break;
            }
        }
        id++; 
    }
    fclose(arquivo);
    
    return id;
}

void printCartas() {
    printf("ASSASSINOS\n");
    printAssassinos();
    printf(GREEN"--------------------------\n"RESET);
    printf("ARMAS\n");
    printArmas();
    printf(GREEN"\n--------------------------\n"RESET);
    printf("LOCAIS\n");
    printLocais();
    printf("\n");
    int comando = 0;
    while (comando != 1) {
        printf("\nDigite '1' para continuar:");
        printf("\n");
        scanf("%d", &comando);
    }
    clearScreen();
}