#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 10


// ======================================================
// ESTRUTURA DA ÁRVORE DE SALAS
// ======================================================
typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala *esq;
    struct Sala *dir;
} Sala;


// ======================================================
// ESTRUTURA DO NÓ DA BST DE PISTAS
// ======================================================
typedef struct NodePista {
    char pista[100];
    struct NodePista *esq;
    struct NodePista *dir;
} NodePista;


// ======================================================
// ESTRUTURA DA TABELA HASH (CHAVE = pista, VALOR = suspeito)
// ======================================================
typedef struct HashNode {
    char pista[100];
    char suspeito[50];
} HashNode;

HashNode tabela[TAM_HASH];


// ======================================================
// FUNÇÃO HASH (soma ASCII simples)
// ======================================================
int hashFunc(char *str) {
    int soma = 0;
    for (int i = 0; str[i] != '\0'; i++) soma += str[i];
    return soma % TAM_HASH;
}


// ======================================================
// INSERIR NA TABELA HASH
// ======================================================
void inserirHash(char *pista, char *suspeito) {
    int h = hashFunc(pista);

    // tratamento simples de colisão linear
    while (strlen(tabela[h].pista) != 0) {
        h = (h + 1) % TAM_HASH;
    }

    strcpy(tabela[h].pista, pista);
    strcpy(tabela[h].suspeito, suspeito);
}


// ======================================================
// BUSCAR SUSPEITO DE UMA PISTA
// ======================================================
char* buscarSuspeito(char *pista) {
    int h = hashFunc(pista);

    for (int i = 0; i < TAM_HASH; i++) {
        int idx = (h + i) % TAM_HASH;
        if (strcmp(tabela[idx].pista, pista) == 0)
            return tabela[idx].suspeito;
    }

    return NULL;
}


// ======================================================
// CRIAR SALA (ALOCADO DINAMICAMENTE)
// ======================================================
Sala* criarSala(char nome[], char pista[]) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esq = nova->dir = NULL;
    return nova;
}


// ======================================================
// INSERIR PISTA NA BST
// ======================================================
NodePista* inserirBST(NodePista *raiz, char pista[]) {
    if (raiz == NULL) {
        NodePista *novo = (NodePista*) malloc(sizeof(NodePista));
        strcpy(novo->pista, pista);
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirBST(raiz->esq, pista);
    else
        raiz->dir = inserirBST(raiz->dir, pista);

    return raiz;
}


// ======================================================
// IMPRIMIR PISTAS EM ORDEM ALFABÉTICA
// ======================================================
void imprimirBST(NodePista *raiz) {
    if (raiz == NULL) return;

    imprimirBST(raiz->esq);
    printf("- %s\n", raiz->pista);
    imprimirBST(raiz->dir);
}


// ======================================================
// EXPLORAÇÃO INTERATIVA DA MANSÃO
// ======================================================
void explorar(Sala *atual, NodePista **raizPistas) {
    char escolha;

    while (1) {
        printf("\nVocê entrou em: %s\n", atual->nome);

        // Coletar pista, se houver
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *raizPistas = inserirBST(*raizPistas, atual->pista);
        } else {
            printf("(Esta sala não contém pistas)\n");
        }

        // Se não há caminhos, parar
        if (atual->esq == NULL && atual->dir == NULL) {
            printf("\nFim do caminho. Exploração encerrada.\n");
            return;
        }

        printf("\nEscolha o caminho (e/d/s): ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esq != NULL) atual = atual->esq;
            else printf("Não há sala à esquerda.\n");
        }
        else if (escolha == 'd' || escolha == 'D') {
            if (atual->dir != NULL) atual = atual->dir;
            else printf("Não há sala à direita.\n");
        }
        else if (escolha == 's' || escolha == 'S') {
            printf("Exploração interrompida pelo jogador.\n");
            return;
        }
        else {
            printf("Opção inválida.\n");
        }
    }
}


// ======================================================
// CONTAR QUANTAS PISTAS APONTAM PARA UM SUSPEITO
// ======================================================
int contarPistas(NodePista *raiz, char *suspeito) {
    if (raiz == NULL) return 0;

    int total = 0;

    char *s = buscarSuspeito(raiz->pista);
    if (s != NULL && strcmp(s, suspeito) == 0)
        total++;

    total += contarPistas(raiz->esq, suspeito);
    total += contarPistas(raiz->dir, suspeito);

    return total;
}


// ======================================================
// PROGRAMA PRINCIPAL
// ======================================================
int main() {

    // ================= CRIAR MANSÃO =================
    Sala *hall        = criarSala("Hall de Entrada",          "A porta foi arrombada");
    Sala *salaEstar   = criarSala("Sala de Estar",            "Pegadas de lama");
    Sala *cozinha     = criarSala("Cozinha",                  "");
    Sala *biblioteca  = criarSala("Biblioteca",               "Livro fora do lugar");
    Sala *jardim      = criarSala("Jardim",                   "Flores pisoteadas");
    Sala *porao       = criarSala("Porão",                    "Caixa mexida recentemente");

    hall->esq = salaEstar;
    hall->dir = cozinha;

    salaEstar->esq = biblioteca;
    salaEstar->dir = jardim;

    cozinha->esq = porao;

    // ================= CRIAR TABELA HASH =================
    // pista → suspeito
    inserirHash("A porta foi arrombada", "Carlos");
    inserirHash("Pegadas de lama", "Maria");
    inserirHash("Livro fora do lugar", "Carlos");
    inserirHash("Flores pisoteadas", "João");
    inserirHash("Caixa mexida recentemente", "Carlos");

    // ================= EXPLORAÇÃO =================
    NodePista *raizPistas = NULL;

    printf("\n===== DETECTIVE QUEST: CAPÍTULO FINAL =====\n");
    explorar(hall, &raizPistas);

    // ================= LISTAR PISTAS COLETADAS =================
    printf("\n===== PISTAS COLETADAS (ordem alfabética) =====\n");

    if (raizPistas == NULL) {
        printf("Nenhuma pista coletada.\n");
    } else {
        imprimirBST(raizPistas);
    }


    // ================= ACUSAÇÃO FINAL =================
    char suspeito[50];
    printf("\nQuem é o culpado? ");
    scanf(" %[^\n]s", suspeito);

    int qtd = contarPistas(raizPistas, suspeito);

    if (qtd >= 2) {
        printf("\nACUSAÇÃO CORRETA! %s foi realmente o culpado.\n", suspeito);
        printf("Pistas confirmando: %d\n", qtd);
    } else {
        printf("\nACUSAÇÃO INCORRETA! Não há pistas suficientes contra %s.\n", suspeito);
        printf("Pistas encontradas: %d\n", qtd);
    }

    printf("\nObrigado por jogar Detective Quest!\n");

    return 0;
}