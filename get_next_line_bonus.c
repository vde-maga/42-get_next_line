/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vde-maga <vde-maga@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 13:17:43 by vde-maga          #+#    #+#             */
/*   Updated: 2025/05/12 16:29:18 by vde-maga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static char	*ft_get_buffer(int fd, char *stash)
{
	char	*buffer;
	ssize_t	bytes_read;
	char	*temp;

	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
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
}

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

char	*get_next_line(int fd)
{
	static char	*stash[1024];
	char		*line;

	if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0)
		return (NULL);
	stash[fd] = ft_get_buffer(fd, stash[fd]);
	if (!stash[fd])
		return (NULL);
	line = ft_get_line(stash[fd]);
	stash[fd] = ft_get_next(stash[fd]);
	return (line);
}

#include <stdio.h>    // para printf() e perror()
#include <stdlib.h>   // para free() e exit()
#include <fcntl.h>    // para open()
#include <unistd.h>   // para close()
#include "get_next_line_bonus.h" // inclua o header da implementação bonus

int main(int argc, char **argv)
{
    int fd1, fd2;
    char *line_fd1;
    char *line_fd2;

    if (argc != 3)
    {
        printf("Uso: %s <arquivo1> <arquivo2>\n", argv[0]);
        return (1);
    }

    fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1)
    {
        perror("Erro ao abrir o arquivo 1");
        return (1);
    }
    fd2 = open(argv[2], O_RDONLY);
    if (fd2 == -1)
    {
        perror("Erro ao abrir o arquivo 2");
        close(fd1);
        return (1);
    }

    /*
     * O laço a seguir chama get_next_line de cada descritor.
     * Como a implementação bonus utiliza um array estático indexado por fd,
     * é possível ler simultaneamente de vários arquivos.
     * O laço intercalará a impressão das linhas de cada arquivo.
     */
    while (1)
    {
        line_fd1 = get_next_line(fd1);
        line_fd2 = get_next_line(fd2);

        if (!line_fd1 && !line_fd2)
            break;

        if (line_fd1)
        {
            printf("Arquivo 1: %s", line_fd1);
            free(line_fd1);
        }
        if (line_fd2)
        {
            printf("Arquivo 2: %s", line_fd2);
            free(line_fd2);
        }
    }

    close(fd1);
    close(fd2);
    return (0);
}

