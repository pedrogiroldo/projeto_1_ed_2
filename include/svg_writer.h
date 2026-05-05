#ifndef SVG_WRITER_H
#define SVG_WRITER_H

/*
 * Gerador incremental de arquivos SVG.
 *
 * Crie com svg_writer_criar(), adicione elementos com as funções de desenho e
 * finalize com svg_writer_finalizar() para fechar o tag raiz e salvar.
 * svg_writer_destruir() libera a memória independentemente de finalizar.
 */

typedef struct svg_writer svg_writer_t;

/*
 * Abre output_path para escrita. O viewBox final é calculado dinamicamente
 * a partir dos elementos desenhados, com margem fixa. largura e altura são
 * usados apenas como fallback quando nenhum elemento é desenhado.
 * Retorna NULL se output_path for NULL ou se o arquivo não puder ser aberto.
 */
svg_writer_t *svg_writer_criar(const char *output_path,
                               double largura, double altura);

/* Fecha o elemento raiz <svg> e fecha o arquivo. Aceita NULL com segurança. */
void svg_writer_finalizar(svg_writer_t *sw);

/* Libera a memória. Não chama finalizar; use svg_writer_finalizar() antes. */
void svg_writer_destruir(svg_writer_t *sw);

/* Desenha um retângulo preenchido. */
void svg_writer_retangulo(svg_writer_t *sw,
                          double x, double y, double w, double h,
                          const char *fill, const char *stroke,
                          double stroke_width);

/* Desenha um retângulo na camada base, escrita antes das marcações. */
void svg_writer_retangulo_base(svg_writer_t *sw,
                               double x, double y, double w, double h,
                               const char *fill, const char *stroke,
                               double stroke_width);

/* Escreve texto na posição (x, y). */
void svg_writer_texto(svg_writer_t *sw, double x, double y,
                      const char *texto, const char *font_size,
                      const char *fill);

/* Marca a posição com um X vermelho de tamanho 'tamanho'. */
void svg_writer_x_vermelho(svg_writer_t *sw,
                           double x, double y, double tamanho);

/* Marca uma quadra removida com diagonais ligando seus vértices opostos. */
void svg_writer_x_quadra_removida(svg_writer_t *sw,
                                  double x, double y, double w, double h);

/* Desenha uma cruz vermelha (+ vertical/horizontal) centrada em (x, y). */
void svg_writer_cruz_vermelha(svg_writer_t *sw,
                              double x, double y, double tamanho);

/* Desenha um círculo preto preenchido centrado em (x, y). */
void svg_writer_circulo_preto(svg_writer_t *sw,
                              double x, double y, double raio);

/* Desenha um quadrado vermelho com o CPF escrito dentro. */
void svg_writer_quadrado_cpf(svg_writer_t *sw,
                             double x, double y, double lado,
                             const char *cpf);

#endif
