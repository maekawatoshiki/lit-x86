# reproduce using Class..

require "std"

struct Person
  name:string
  age
  friends:Person[]
end

module Person
  def new(name:string, age):Person
    person = new Person
    person.name = name
    person.age = age
    person.friends = []:Person
    person
  end
end

def add_friend(p:Person, f:Person)
  size = p.friends.builtinlength()
  friends = new Person size+1
  for i in 0...size
    friends[i] = p.friends[i]
  end
  friends[size] = f
  p.friends = friends
end

def get_friends(p:Person):string[]
  s = []:string
  for i in 0...p.friends.builtinlength()
    s += p.friends[i].name
  end
  s
end

def intro(p:Person)
  puts("My name is {}. I have {} friends." % p.name % p.friends.builtinlength())
end

kevin = Person::new("Kevin", 15)
sakura = Person::new("Sakura", 18)
deepa = Person::new("Deepa", 14)
kevin.add_friend(sakura)
kevin.add_friend(deepa)
deepa.add_friend(sakura)

kevin.intro()
puts( kevin.get_friends() )

deepa.intro()
puts( deepa.get_friends() )
