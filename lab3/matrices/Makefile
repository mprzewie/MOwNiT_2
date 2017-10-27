sos=-lgsl -lgslcblas -lm
bin_dir=bin/

all:
	gcc matrices.c ${sos} -o ${bin_dir}matrices

optimized:
	gcc matrices.c -Ofast ${sos} -o ${bin_dir}matrices_optimized

clean:
	rm ${bin_dir}*
