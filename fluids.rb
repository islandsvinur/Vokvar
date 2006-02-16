#!/usr/bin/ruby

require 'opengl'
require 'glut'
require 'mathn'

# Numerical N-dimensional array (narray-0.5.8.tar.gz)
require 'narray'
# FFTW wrapper library (ruby-fftw3-0.2.tar.gz)
require 'numru/fftw3'

class Simulation
  def initialize(n = 1)
    dim = n * 2  * (n/2+1)
  end

  def set_color(x, y, color)
    
  end

  def set_forces ; end
  def stable_solve ; end
  def diffuse_matter ; end

end # Simulation

class Visualization
  def initialize(simulation)
    @sim = simulation
    GLUT.Init
    GLUT.InitDisplayMode(GLUT::SINGLE | GLUT::RGB | GLUT::DEPTH);
    GLUT.InitWindowSize(500, 500);
    GLUT.CreateWindow("Real-time wispy smoke");

    GL.ClearColor(0.0, 0.0, 0.0, 0.0)
    GL.MatrixMode(GL::PROJECTION)
    GL.LoadIdentity()
    GL.Ortho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0)

    GLUT.DisplayFunc(proc { display });
    GLUT.ReshapeFunc(proc { reshape });
    GLUT.IdleFunc(proc { idle });
    GLUT.KeyboardFunc(proc { |k,x,y| keyboard(k,x,y) });
    GLUT.MotionFunc(proc { drag });
  end

  def start
    GLUT.MainLoop();
  end
  
  def draw_field
  end

  def display
    GL.Clear(GL::COLOR_BUFFER_BIT)

    GL.Color(1.0, 1.0, 1.0)
    GL.Begin(GL::POLYGON)
      GL.Vertex(0.25, 0.25, 0.0)
      GL.Vertex(0.75, 0.25, 0.0)
      GL.Vertex(0.75, 0.75, 0.0)
      GL.Vertex(0.25, 0.75, 0.0)
    GL.End()

    GL.Flush()
  end

  def idle
    unless $frozen
      @sim.set_forces
      @sim.stable_solve
      @sim.diffuse_matter
      GLUT.PostRedisplay
    end
  end

  def reshape(width = 0, height = 0)
  end

  def keyboard(key = '', x = 0, y = 0)
    puts "Keypress: #{x} #{y}"
    case key
      # Escape key
      when 27, 'q'[0], 'Q'[0]
        exit(0);
      when 'f'[0]
        $frozen = false
      when 'F'[0]
        $frozen = true
    end
  end

  def drag(mx = 0, my = 0)
  end
end

$frozen = false
Thread.abort_on_exception = true

sim = Simulation.new

viz = Visualization.new(sim)
viz.start

