1. Crio o stash e a line
2. Coloco o conteudo todo recebido no file descriptor, dentro do stash
	1. Aloco o tamanho do buffer mais o nulo num char* buffer
		1. O buffer size e o tamanho defenido de quanto e que queremos o bloco a serem procurados ate ao \n e este e defenido pelo user na hora de executar o codigo
	2. Faco com que bytes_read seja igual a 1, aka, verdadeiro ou ativo
	3. Enquanto bytes_read for veradadeiro (maior que 0, porque ele sera grande) e nao estiveres no caracter da breakline, entao
	4. le o conteudo do fd, coloca-o no buffer e faz com que o tamanho lido, seja igual a bytes_read
	5. Se o comando anterior, entregar -1, significa que cehgamos ao fim do fd e devemos limpar a stash, o buffer e retornar nulo
	6. fazemos com que, na ultima posicao de buffer, seja igual a nulo
	7. O stash atual, vai para temp e depois colocamos a stash a ser igual a temp (stash antiga ) + buffer, que acaba por ser a linha que queremos
	8. Livertada toda a memoria que foi usada de forma temporaria, sendo ela o buffer e temp e depois devolvemos a stash
3. Faco com que line seja igual a primeira linha de stash
	1. Para esse efeito, vou para a funcao get_line
	2. 
