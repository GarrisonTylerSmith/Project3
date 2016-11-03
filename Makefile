all: ray_caster.c
	gcc ray_caster.c -o ray_caster

clean:
	rm -rf ray_caster *~

