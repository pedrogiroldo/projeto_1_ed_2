#ifndef QUADRA_H
#define QUADRA_H

#define QUADRA_CEP_MAX 32u
#define QUADRA_COR_MAX 32u

typedef struct quadra quadra_t;

typedef struct {
  char cep[QUADRA_CEP_MAX + 1u];
  double x;
  double y;
  double largura;
  double altura;
  char cor_preenchimento[QUADRA_COR_MAX + 1u];
  char cor_borda[QUADRA_COR_MAX + 1u];
  double espessura_borda;
} quadra_registro_t;

quadra_t *quadra_criar(const char *cep, double x, double y, double largura,
                       double altura, const char *cor_preenchimento,
                       const char *cor_borda, double espessura_borda);

void quadra_destruir(quadra_t *quadra);

const char *quadra_obter_cep(const quadra_t *quadra);
double quadra_obter_x(const quadra_t *quadra);
double quadra_obter_y(const quadra_t *quadra);
double quadra_obter_largura(const quadra_t *quadra);
double quadra_obter_altura(const quadra_t *quadra);
const char *quadra_obter_cor_preenchimento(const quadra_t *quadra);
const char *quadra_obter_cor_borda(const quadra_t *quadra);
double quadra_obter_espessura_borda(const quadra_t *quadra);
int quadra_para_registro(const quadra_t *quadra, quadra_registro_t *registro);
quadra_t *quadra_criar_de_registro(const quadra_registro_t *registro);

#endif
