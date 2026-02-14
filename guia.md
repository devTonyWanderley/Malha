Resumo de Concepção: Projeto Malha (C++20)
1. Unidade e Precisão
Escala: Coordenadas inteiras em décimos de milímetro (
metrôs).
Representação: uint32_t para coordenadas locais (
).
Alcance: Até 429,4 km de extensão local (suficiente para topografia/obras).
Tradução: Uso de x0, y0( uint64_t) globalmente para conversor UTM -> Local.
2. Estrutura do Ponto (Atributo)
O Ponto não é mais uma classe base com herança, mas um atributo de outras abstrações
(Ex: Amostra.posicao).
Campos: x, y, z (locais) e chave_espacial(Código Morton/DNA).
3. Indexação Espacial (Código Morton / BMI2)

Lógica: Intercalação de bits de
e
para gerar uma chave de 64 bits.
Ferramenta: Instrução intrínseca _pdep_u64(IMC2) para desempenho máximo.
4. Estrutura da Quadtree (Esparsa)
Divisão: Baseada nos bits da chave_espacial (2 bits por nível).
Eficiência: Não alocar nós para espaços vazios (Lazy Allocation).
Profundidade: Adaptativa. Não subdividir além da necessidade
(ex: se o quadro tiver apenas 1 ponto ou atingir o limite de vizinhança de 20m).
5. Ambiente de Desenvolvimento
Build: CMake (Projeto Raiz + Subprojetos Core/IO).
Padrão: C++20 (std::ranges, std::variant, operator<=>).
Flags Necessárias: -std=c++20 e -mbmi2.
quatro dicas de ouro para nossa próxima sessão:
_pdep_u64
Use o "Modo Revisor": Antes de copiar um código, pergunte à IA:
"Quais são os pontos fracos dessa abordagem?" .
Isso obriga a IA a revelar limitações que ela normalmente esconderia para parecer "eficiente".
Mantenha o Glossário: Termos como BMI2 , Morton , C++20 Ranges e Shared Libraries são os
pilares do seu projeto.
Se eu usar um termo técnico que seja "grego", pare tudo e peça uma analogia.
Dica Final: Trate a IA como um estagiário super dotado, mas sem bom senso .
Ele conhece todos os livros da biblioteca, mas você é o Engenheiro Chefe que sabe por onde a
estrada deve passar.

    --Refatoração de geometria.h 14/02/26 16:09--
dividir em geometria.h & core.h

fica em geometria, namespace Geo:
struct ConfigCenario
struct alignas(32) Ponto
struct Amostra

vai para massadados, namespace Massa:
class GerenciadorMassa
