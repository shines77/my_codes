
    if (errno == ENOENT || errno == EACCES || errno == ECONNREFUSED) {
        /* ENOENT       : Socket may not have shown up yet
         * EACCES       : acces is not yet granted
         * ECONNREFUSED : Leftover socket hasn't been removed yet */
        continue;
    }
