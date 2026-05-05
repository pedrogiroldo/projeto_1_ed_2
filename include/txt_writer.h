#ifndef TXT_WRITER_H
#define TXT_WRITER_H

/*
 * Gravador incremental de arquivo de texto para respostas de consultas.
 *
 * Crie com txt_writer_criar(), adicione linhas com txt_writer_linha() e
 * libere com txt_writer_destruir() (que fecha o arquivo automaticamente).
 */

typedef struct txt_writer txt_writer_t;

/*
 * Abre output_path para escrita.
 * Retorna NULL se output_path for NULL ou se o arquivo não puder ser aberto.
 */
txt_writer_t *txt_writer_criar(const char *output_path);

/* Fecha o arquivo e libera a memória. Aceita NULL com segurança. */
void txt_writer_destruir(txt_writer_t *tw);

/*
 * Escreve uma linha formatada seguida de newline.
 * Aceita os mesmos especificadores que printf.
 * Chamada segura mesmo se tw for NULL (ignorada).
 */
void txt_writer_linha(txt_writer_t *tw, const char *fmt, ...);

#endif
