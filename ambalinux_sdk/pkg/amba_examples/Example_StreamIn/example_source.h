struct example_source_s;

int example_source_open(struct example_source_s **source, const char* url);
int example_source_read(struct example_source_s *source, unsigned char**, unsigned int*);
int example_source_close(struct example_source_s *source);
