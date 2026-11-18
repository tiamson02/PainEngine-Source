CPCOMMAND=cp -a

debug:
	$(MAKE) -C ps2stuff debug PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2stuff/objs_debug/libps2stuff.a builtlibs
	$(MAKE) -C ps2gl debug PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2gl/objs_debug/libps2gl.a builtlibs
	$(MAKE) -C ps2vecmat debug PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2vecmat/objs_debug/libps2vecmat.a builtlibs
	$(MAKE) -C MultiStream/ee
	$(MAKE) -C MultiStream/iop
	$(CPCOMMAND) MultiStream/iop/stream.irx ../../example/iop
	$(MAKE) -C ps2al PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2al/libps2al.a builtlibs
	$(MAKE) -C ps2shell PROJECTDEFS="$(PROJECTDEFS)"
	$(MAKE) -C shellMCUI PROJECTDEFS="$(PROJECTDEFS)"
	@-touch builtlibs/optimized
	@-touch builtlibs/release
	@-rm builtlibs/optimized
	@-rm builtlibs/release
	@-touch builtlibs/debug

optimized:
	$(MAKE) -C ps2stuff optimized PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2stuff/objs_optimized/libps2stuff.a builtlibs
	$(MAKE) -C ps2gl optimized PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2gl/objs_optimized/libps2gl.a builtlibs
	$(MAKE) -C ps2vecmat optimized PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2vecmat/objs_optimized/libps2vecmat.a builtlibs
	$(MAKE) -C MultiStream/ee
	$(MAKE) -C MultiStream/iop
	$(CPCOMMAND) MultiStream/iop/stream.irx ../../example/iop
	$(MAKE) -C ps2al PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2al/libps2al.a builtlibs
	$(MAKE) -C ps2shell PROJECTDEFS="$(PROJECTDEFS)"
	$(MAKE) -C shellMCUI PROJECTDEFS="$(PROJECTDEFS)"
	@-touch builtlibs/debug
	@-touch builtlibs/release
	@-rm builtlibs/debug
	@-rm builtlibs/release
	@-touch builtlibs/optimized

release:
	$(MAKE) -C ps2stuff release PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2stuff/objs_release/libps2stuff.a builtlibs
	$(MAKE) -C ps2gl release PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2gl/objs_release/libps2gl.a builtlibs
	$(MAKE) -C ps2vecmat release PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2vecmat/objs_release/libps2vecmat.a builtlibs
	$(MAKE) -C MultiStream/ee
	$(MAKE) -C MultiStream/iop
	$(CPCOMMAND) MultiStream/iop/stream.irx ../../example/iop
	$(MAKE) -C ps2al PROJECTDEFS="$(PROJECTDEFS)"
	$(CPCOMMAND) ps2al/libps2al.a builtlibs
	$(MAKE) -C ps2shell PROJECTDEFS="$(PROJECTDEFS)"
	$(MAKE) -C shellMCUI PROJECTDEFS="$(PROJECTDEFS)"
	@-touch builtlibs/debug
	@-touch builtlibs/optimized
	@-rm builtlibs/debug
	@-rm builtlibs/optimized
	@-touch builtlibs/release

clean:
	find . -name "*.irx" -exec rm {} \;
	find . -name "*.o" -exec rm {} \;
	find . -name "*.a" -exec rm {} \;

none:
