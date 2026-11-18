import string

states = ['Attack', 'CloseAttack', 'FarAttack', 'Flee', 'Hear', 'Hunt', 'Idle', 'Jump', 'JumpUp', 'RunToPlayer', 'SpiderJump', 'Throw']

placeholder = 'xxx'
h_template_fname = 'C' + placeholder + 'State.h'
cpp_template_fname = 'C' + placeholder + 'State.cpp'

def instantiate_template(template_name, state_name):
	template = open(template_name).read()

	new_contents = template.replace(placeholder.upper(), state_name.upper())
	new_contents = new_contents.replace(placeholder, state_name)

	new_file = open(template_name.replace(placeholder, state_name), 'w')
	new_file.write(new_contents)
	
if __name__=='__main__':
	for s in states:
		instantiate_template(h_template_fname, s)
		instantiate_template(cpp_template_fname, s)
		
		
		
	
	
	
	