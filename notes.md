1. Crio o stash e a line
2. Coloco o conteudo todo recebido no file descriptor, dentro do stash
	1. Aloco o tamanho do buffer mais o nulo num char* buffer
		1. O buffer size e o tamanho defenido de quanto e que queremos o bloco a serem procurados ate ao \n e este e defenido pelo user na hora de executar o codigo
	2. Faco com que bytes_read seja igual a 1, aka, verdadeiro ou ativo
	3. Enquanto bytes_read for verdadeiro (maior que 0, porque ele sera grande) e nao estiveres no caracter da breakline, entao
	4. le o conteudo do fd, coloca-o no buffer e faz com que o tamanho lido, seja igual a bytes_read
	5. Se o comando anterior, entregar -1, significa que cehgamos ao fim do fd e devemos limpar a stash, o buffer e retornar nulo
	6. fazemos com que, na ultima posicao de buffer, seja igual a nulo
	7. O stash atual, vai para temp e depois colocamos a stash a ser igual a temp (stash antiga ) + buffer, que acaba por ser a linha que queremos
	8. Livertada toda a memoria que foi usada de forma temporaria, sendo ela o buffer e temp e depois devolvemos a stash
3. Faco com que line seja igual a primeira linha de stash
	1. Para esse efeito, vou para a funcao get_line
		1. Percorro toda a stash, até chegar a uma newline
		2. Se onde estou, for uma newline, então, avança uma posição
			1. Desta forma, tenho a certeza que o i também está a apontar para a newline
		3. Faço com que a minha linha local, seja exatamanete igual a stash, até chegar ao fim de i, ou seja, vou fazer com que esta função, retorne stash, até chegar ao fim de uma breakline
4. Atualmente tenho uma linha até ao breakline, mas o meu stash ainda tem esse conteudo
	1. Se assim for, quando o código for rodado outra vez, terei problemas, porque estaria sempre a repetir a primeira linha até à newline, entrando num ciclo infinito
5. Para não entrar num ciclo infinito, eu faço com que stash, seja apaga e consiga o proximo conteudo
	1. Crio um array next, que será igual a stash, mas só com o next.
		1. Seguindo uma lógica similar, percorro a stash, até chegar a uma breakline
		2. Depois, se onde estou, for uma breakline, então, anda mais um
		3. Se já cheguei ao fim da stash, ou seja, ela é falta, então dá free à mesma e retorna null?
		4. Com o i a apontar para uma breakline, então, faço com next, seja uma cópia de stash, até ao i
		5. Por fim, retorno o next



A seguir, apresento uma explicação por etapas do código, enfatizando as decisões de implementação, as condições e o que cada trecho faz, para que possas ter uma visão completa e profunda do funcionamento do exercício *get_next_line*:

---

### 1. Variáveis e Alocação de Memória para o Buffer

```c
buffer = malloc(BUFFER_SIZE + 1);
if (!buffer)
    return (NULL);
```

- **Objetivo:**  
  Alocar um bloco de memória para armazenar temporariamente os dados lidos do file descriptor.  
- **Detalhes importantes:**  
  - O tamanho é `BUFFER_SIZE + 1` para que, após a leitura, seja possível adicionar um caractere nulo (`'\0'`) no final da string, garantindo que ela esteja bem terminada.
  - Se `malloc` falha, o código retorna `NULL`, pois não há como proceder sem o buffer.

---

### 2. Acumulação dos Dados Lidos (*ft_get_buffer*)

```c
bytes_read = 1;
while (bytes_read > 0 && !ft_strchr(stash, '\n'))
{
    bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read == -1)
        return (free(stash), free(buffer), NULL);
    buffer[bytes_read] = '\0';
    temp = stash;
    stash = ft_strjoin(temp, buffer);
    if (temp)
        free(temp);
}
free(buffer);
return (stash);
```

- **Objetivo:**  
  Ler os dados do file descriptor (fd) de forma iterativa, acumulando-os na variável estática `stash` até que se encontre uma quebra de linha (`'\n'`) ou o fim do arquivo seja alcançado.
  
- **Etapas e condições:**
  - **Inicialização de `bytes_read`:**  
    Definir `bytes_read = 1` garante que entremos no loop inicialmente, pois o valor inicial precisa ser maior que 0.
    
  - **Condição do While:**  
    `while (bytes_read > 0 && !ft_strchr(stash, '\n'))` significa:
    - Continuar lendo enquanto:
      - **`bytes_read > 0`:** Significa que a última operação de leitura foi bem-sucedida e ainda há dados para ler (retorno de `read()` diferente de 0 indica que ainda não chegamos ao final).
      - **`!ft_strchr(stash, '\n')`:** Significa que, até o momento, a cadeia acumulada em `stash` não contém um caractere de quebra de linha, ou seja, ainda não temos uma linha completa.
    
  - **Leitura com `read`:**  
    Lê até `BUFFER_SIZE` bytes. Se ocorrer um erro (quando `read()` retorna -1), libera tanto o `buffer` quanto o `stash` (para evitar vazamentos de memória) e retorna `NULL`.  
    **Observação:**  
    - Um retorno de `-1` indica um erro de leitura e não o fim do arquivo. O fim é sinalizado quando `read()` retorna 0.
    
  - **Tratamento da String Lida:**  
    Após cada leitura:
    - É colocado o caractere nulo na posição `buffer[bytes_read]` para garantir que o buffer seja uma string válida.
    - O conteúdo lido é concatenado ao conteúdo já existente em `stash` usando `ft_strjoin`.  
    - O ponteiro antigo (`temp`) é liberado, pois a memória alocada anteriormente para `stash` não será mais necessária.
  
  - **Finalização:**  
    Após sair do loop (ocorrendo quando encontramos uma quebra de linha ou nenhum byte é lido), o `buffer` é liberado e retorna-se o `stash` atualizado.  
      
Esse procedimento garante que `stash` acumule dados suficientes para extrair pelo menos uma linha completa.

---

### 3. Extração da Linha Completa (*ft_get_line*)

```c
static char	*ft_get_line(char *stash)
{
    char	*line;
    size_t	i;

    i = 0;
    if (!stash || stash[0] == '\0')
        return (NULL);
    while (stash[i] && stash[i] != '\n')
        i++;
    if (stash[i] == '\n')
        i++;
    line = ft_substr(stash, 0, i);
    return (line);
}
```

- **Objetivo:**  
  Gerar uma nova string contendo a primeira linha completa presente em `stash`.

- **Etapas e condições:**
  - **Validação Inicial:**  
    Se `stash` for `NULL` ou estiver vazio (`stash[0] == '\0'`), retorna `NULL` — isso pode ocorrer se não houver dados para ler.
  
  - **Percorrer até o Final da Linha:**  
    Um loop percorre `stash` até encontrar:
    - O caractere nulo (fim da string), ou
    - A quebra de linha (`'\n'`).
  
  - **Incluir a Quebra de Linha:**  
    Se encontrou uma quebra de linha (`stash[i] == '\n'`), incremente `i` para que a substring resultante inclua esse caractere, atendendo ao que geralmente se espera da função get_next_line (ou seja, retornar linhas que terminam com `'\n'`, se houver).
  
  - **Criação da Substring:**  
    Utiliza `ft_substr` para copiar os primeiros `i` caracteres de `stash` para a nova string `line`. Essa nova string é o que será retornado como a linha completa lida.
  
Essa etapa isola a linha a ser retornada sem modificar o conteúdo original do `stash`; ela serve apenas para extrair a parte que queremos mostrar.

---

### 4. Atualização do *Stash* para o Próximo Chamado (*ft_get_next*)

```c
static char	*ft_get_next(char *stash)
{
    char	*next;
    size_t	i;

    i = 0;
    if (!stash)
        return (NULL);
    while (stash[i] && stash[i] != '\n')
        i++;
    if (stash[i] == '\n')
        i++;
    if (!stash[i])
    {
        free(stash);
        return (NULL);
    }
    next = ft_strdup(stash + i);
    if (!next)
        return (free(stash), NULL);
    free(stash);
    return (next);
}
```

- **Objetivo:**  
  Remover a parte de `stash` que já foi retornada e manter apenas o conteúdo que ainda não foi processado para as próximas chamadas a `get_next_line`.

- **Etapas e condições:**
  - **Percorrer até o Final da Linha Atual:**  
    Um loop avança o índice `i` até encontrar a quebra de linha. Se encontrar, incrementa `i` para começar exatamente após essa quebra.
    
  - **Verificação de Conteúdo Remanescente:**  
    Se, após essa operação, o caractere atual (`stash[i]`) for o nulo, significa que não há mais conteúdo restante. Nesse caso, a memória de `stash` é liberada e é retornado `NULL`, indicando que não há mais dados para ler no futuro.
    
  - **Criação do Novo Stash:**  
    Caso contrário, é usado `ft_strdup` para duplicar a parte de `stash` que vem depois do índice `i` (ou seja, a parte não lida anteriormente).  
    - Se a duplicação falhar, libera `stash` e retorna `NULL` para evitar vazamentos.
  
  - **Liberação da Memória Antiga:**  
    Após criar a nova string `next`, liberta-se o `stash` antigo para não perder a referência e causar vazamentos.
  
Essa função garante que, após extrair uma linha completa, o *stash* seja atualizado para conter somente o que falta ler, preparando o terreno para a próxima chamada de `get_next_line`.

---

### 5. Função Principal *get_next_line*

```c
char	*get_next_line(int fd)
{
    static char	*stash;
    char		*line;

    if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0)
        return (NULL);
    stash = ft_get_buffer(fd, stash);
    if (!stash)
        return (NULL);
    line = ft_get_line(stash);
    stash = ft_get_next(stash);
    return (line);
}
```

- **Objetivo:**  
  Integrar todas as etapas anteriores e garantir que cada chamada à função retorne a próxima linha completa do arquivo.

- **Etapas e condições:**
  - **Validação dos Parâmetros:**  
    - Verifica se o `fd` é um file descriptor válido (não negativo e menor que 1024, um limite comum para evitar comportamentos indefinidos).
    - Garante que `BUFFER_SIZE` seja maior que zero.
    Se qualquer uma dessas condições falhar, retorna imediatamente `NULL`.
    
  - **Acumulação de Dados:**  
    Chama `ft_get_buffer(fd, stash)` para atualizar o `stash` com novas leituras do arquivo, se necessário, até que haja uma linha completa (ou o fim do arquivo seja alcançado).
    - Se `ft_get_buffer` retornar `NULL`, isso pode significar um erro de leitura ou que não há mais dados, e a função retorna `NULL`.
  
  - **Extração da Linha:**  
    Utiliza `ft_get_line` para isolar e extrair a primeira linha completa presente em `stash`.
  
  - **Atualização do Stash:**  
    Após extrair a linha, chama `ft_get_next` para remover da variável `stash` a parte que já foi retornada, garantindo que na próxima chamada só seja processado o conteúdo remanescente.
  
  - **Retorno:**  
    Retorna a linha extraída, que já pode incluir a quebra de linha (caso ela exista), conforme a especificação do exercício.

---

### Considerações Gerais e Detalhes de Design

1. **Uso da Variável `static char *stash`:**  
   - O uso de `static` permite que a variável mantenha seu valor entre chamadas sucessivas de `get_next_line()`. Assim, o que foi lido mas não processado numa chamada anterior pode ser utilizado na próxima, evitando a necessidade de reler dados já carregados em memória.
  
2. **Tratamento de Erros e Gerenciamento de Memória:**  
   - Cada função cuida de liberar a memória temporária em caso de erro (por exemplo, se `read()` retornar -1 ou se alguma alocação falhar).  
   - Isso é crucial para evitar vazamentos de memória, especialmente num contexto de leitura repetitiva e acumulativa.
  
3. **Decisões de Condicionais:**  
   - **`while (bytes_read > 0 && !ft_strchr(stash, '\n'))`:**  
     Essa condição faz com que a função leia repetidamente até que seja encontrado o caracter de quebra de linha ou que não haja mais dados para ler (quando `read()` retorna 0).  
   - **Inclusão do `'\n'` na Linha:**  
     Ao incrementar o índice logo após encontrar a quebra de linha, a função `ft_get_line` garante que a linha retornada respeite o formato esperado (ou seja, incluir o `'\n'` se presente).
   - **Verificação do Conteúdo Remanescente no `stash`:**  
     Em `ft_get_next`, se não houver conteúdo após a quebra de linha, a memória associada a `stash` é liberada, sinalizando que não há mais dados que possam ser retornados futuramente.
   
4. **Fluxo Geral do Código:**  
   - **Inicio:** O código começa validando os parâmetros e, se tudo estiver correto, inicia a leitura acumulada do arquivo através de `ft_get_buffer()`.
   - **Intermediário:** Em seguida, com os dados acumulados, isola a linha completa atual via `ft_get_line()` e atualiza o `stash` para remover o que já foi processado, usando `ft_get_next()`.
   - **Final:** Por fim, retorna a linha completa lida, deixando o `stash` preparado para a próxima chamada da função.  
  
Esse fluxo modularizado promove uma separação clara de responsabilidades, facilita a manutenção e a detecção de possíveis falhas (por exemplo, erros de leitura ou falhas na alocação de memória).

---

### Conclusão e Reflexões Adicionais

A implementação apresentada segue uma lógica robusta para ler linhas de um arquivo de forma incremental. Cada parte do código é projetada para:

- **Manter o Acúmulo dos Dados:**  
  Através do `stash`, os dados lidos permanecem disponíveis entre chamadas, o que é especialmente útil quando a quebra de linha não está presente em uma única leitura.

- **Evitar Vazamentos de Memória:**  
  O gerenciamento cuidadoso das variáveis temporárias (`buffer` e `temp`) e a liberação apropriada da memória do `stash` garantem que, mesmo em situações de erro, a memória seja liberada.

- **Fornecer uma Interface Consistente:**  
  Toda a lógica é encapsulada dentro de funções bem definidas, onde cada uma tem a responsabilidade de lidar com uma parte específica do problema — ler do arquivo, extrair a linha e atualizar o conteúdo restante. Essa modularização torna o código mais legível e mais fácil de depurar.

É interessante notar que essa abordagem permite que o código lide tanto com arquivos que possuem quebras de linha regulares quanto com arquivos que podem não terminar com uma quebra (nesse caso, a última chamada retornará o que restou). Além disso, a verificação dos parâmetros logo no início de `get_next_line` protege contra uso indevido ou situações em que os parâmetros não fazem sentido.

Se quiseres aprofundar ainda mais, podemos discutir alternativas de concatenação de strings (pois `ft_strjoin` é chamado repetidamente – o que pode ter implicações de desempenho em casos de arquivos muito grandes) ou explorar como implementar um buffer circular para reduzir alocações dinâmicas frequentes.

Qual dos aspectos te interessa mais aprofundar – o gerenciamento de memória e eficiência ou abordagens alternativas para leitura de arquivos?
