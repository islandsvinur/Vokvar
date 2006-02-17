#!/usr/bin/ruby
#
require 'pp'

require 'opengl'
require 'glut'
require 'mathn'

# Numerical N-dimensional array (narray-0.5.8.tar.gz)
require 'narray'
# FFTW wrapper library (ruby-fftw3-0.2.tar.gz)
require 'numru/fftw3'

class Simulation
  attr_reader :velocity_field, :dimension, :smoke_density

  def initialize(dimension = 50)
    @dimension = dim = dimension

    @timestep = 0.004
    @viscosity = 0.001

    # Create dim x dim plane, with 2 dimensional data
    @velocity_field     = NArray.float(dim + 1, dim + 1, 2).random(40.0) - 20.0 # called u, v
    @new_velocity       = NArray.float(dim + 1, dim + 1, 2).fill(0.0) # called u0, v0

    @smoke_density      = NArray.float(dim, dim, 2).fill(0.0) # called rho
    @new_smoke_density  = NArray.float(dim, dim, 2).fill(0.0) # called rho0

    @user_velocity      = NArray.float(dim, dim, 2).fill(0.0) # called u_u0, u_v0
  end

  def set_forces
    @new_smoke_density = @smoke_density * 0.995
    @new_velocity = @user_velocity = @user_velocity * 0.85
  end

  def stable_solve

    n = @dimension 
    v = @velocity_field
    v0 = @new_velocity
    visc = @viscosity
    dt = @timestep

    n**2.times do |i|
      v[i] += v0[i] * dt
      v0[i] = v[i]
    end

    x = 0.5 / n
    y = 0.5 / n

    n.times do |i| n.times do |j|

      x0 = n * (x - dt * v0[i + n * j]) - 0.5
      i0 = x0.floor
      s = x0 - i0
      i0 = (n + (i0 % n)) % n
      i1 = (i0 + 1) % n

      y0 = n * (y - dt * v0[i + n * j + n**2]) - 0.5
      j0 = y0.floor
      t = y0 - j0
      j0 = (n + (j0 % n)) % n
      j1 = (j0 + 1) % n
   
      v[i + n * j] = 
          (1-s) * ((1-t) * v0[i0 + n * j0] + t * v0[i0 + n * j1])
            + s * ((1-t) * v0[i1 + n * j0] + t * v0[i1 + n * j1])
      v[i + n * j + n**2] = 
          (1-s) * ((1-t) * v0[i0 + n * j0 + n**2] + t * v0[i0 + n * j1 + n**2])
            + s * ((1-t) * v0[i1 + n * j0 + n**2] + t * v0[i1 + n * j1 + n**2])

      y += 1.0 / n
      end 
      x += 1.0 / n
    end

    # n.times do |i| n.times do |j|
      # v0[i + (n + 2) * j] = v[i + n * j]
      # v0[i + (n + 2) * j + n**2] = v[i + n * j + n**2]
    # end end
    v0 = v

    v0 = NumRu::FFTW3.fft(v0, 1)
    
  end

  def diffuse_matter
    n = @dimension 
    dt = @timestep
    v = @velocity_field
    rho0 = @new_smoke_density
    x = 0.5 / n
    y = 0.5 / n

    n.times do |i| n.times do |j|
      x0 = n * (x - dt * v[i + n * j]) - 0.5
      i0 = x0.floor
      s = x0 - i0
      i0 = (n + (i0 % n)) % n
      i1 = (i0 + 1) % n

      y0 = n * (y - dt * v[i + n * j + n**2]) - 0.5
      j0 = y0.floor
      t = y0 - j0
      j0 = (n + (j0 % n)) % n
      j1 = (j0 + 1) % n
   
      @smoke_density[i + n * j] = 
          (1-s) * ((1-t) * rho0[i0 + n * j0] + t * rho0[i0 + n * j1])
            + s * ((1-t) * rho0[i1 + n * j0] + t * rho0[i1 + n * j1])

      y += 1.0 / n
      end
      x += 1.0 / n
    end

  end

  def inject_fluid(x, y, dx, dy)
    @user_velocity[y * @dimension + x] += dx
    @user_velocity[@dimension**2 + y * @dimension + x] += dy
    @smoke_density[y * @dimension + x] = 1.0
  end

end # Simulation

class Visualization
  def initialize(simulation)
    @sim = simulation
    @dim = @sim.dimension
    @winsize = 500, 500
    @mousepos = 0, 0
    
    GLUT.Init
    GLUT.InitDisplayMode(GLUT::DOUBLE | GLUT::RGB | GLUT::DEPTH);
    GLUT.InitWindowSize(*@winsize);
    GLUT.CreateWindow("Real-time wispy smoke");

    GLUT.DisplayFunc(proc { display });
    GLUT.ReshapeFunc(proc { |w,h| reshape(w,h) });
    GLUT.IdleFunc(proc { idle });
    GLUT.KeyboardFunc(proc { |k,x,y| keyboard(k,x,y) });
    GLUT.MotionFunc(proc { |mx,my| drag(mx,my) });
  end

  def start
    GLUT.MainLoop();
  end
  
  def point_color(x, y)
    f = Math.atan2(y, x) / Math::PI + 1
    r = f
    if (r > 1) then r = 2 - r end
    g = f + (2.0/3.0)
    if (g > 2) then g = g - 2 end
    if (g > 1) then g = 2 - g end
    b = f + 2 * (2.0/3.0)
    if (b > 2) then b = b - 2 end
    if (b > 1) then b = 2 - b end
    return [r, g, b]
  end

  def draw_field
    draw_smoke
    # draw_hedgehogs
  end

  def draw_hedgehogs
    vf = @sim.velocity_field.to_a
    sz = 2.0 / (@dim + 1.0)
    scale = 0.05 * sz
    colorize = true

    unless colorize
      GL.Color(1.0, 1.0, 1.0)
    end
    GL.Begin(GL::LINES)
      @dim.times do |x| @dim.times do |y|
        if colorize
          GL.Color(*point_color(vf[0][x][y], vf[1][x][y]))
        end
        from = sz - 1.0 + x * sz, 
               sz - 1.0 + y * sz
        to =   sz - 1.0 + x * sz + scale * vf[0][x][y],
               sz - 1.0 + y * sz + scale * vf[1][x][y]
        GL.Vertex(*from)
        GL.Vertex(*to)
      end end
    GL.End
  end

  def draw_smoke
    vf = @sim.velocity_field.to_a
    density = @sim.smoke_density
    sz = 2.0 / (@dim + 1.0)
    scale = 0.05 * sz
    colorize = true

    GL.PolygonMode(GL::FRONT_AND_BACK, GL::FILL)
    @dim.times do |y|
      GL.Begin(GL::TRIANGLE_STRIP)

        px = sz
        py = sz + y * sz
        idx = y * @dim
        GL.Color(density[idx], density[idx], density[idx])
        GL.Vertex(px - 1, 1 - py)

        @dim.times do |x|
          px = sz + x * sz
          py = sz + (y + 1) * sz
          idx = (y + 1) * @dim + x
          GL.Color(density[idx], density[idx], density[idx])
          GL.Vertex(px - 1, 1 - py)

          px = sz + (x + 1) * sz
          py = sz + y * sz
          idx = y * @dim + x + 1
          GL.Color(density[idx], density[idx], density[idx])
          GL.Vertex(px - 1, 1 - py)
        end

        px = sz + (@dim - 1) * sz
        py = sz + (y + 1) * sz
        idx = (y + 1) * @dim + @dim - 1
        GL.Color(density[idx], density[idx], density[idx])
        GL.Vertex(px - 1, 1 - py)
      GL.End
    end
  end

  def display
    GL.Clear(GL::COLOR_BUFFER_BIT | GL::DEPTH_BUFFER_BIT)
    GL.MatrixMode(GL::MODELVIEW)
    GL.LoadIdentity

    draw_field

    GL.Flush
    GLUT.SwapBuffers
  end

  def idle
    unless $frozen
      @sim.set_forces
      @sim.stable_solve
      @sim.diffuse_matter
      GLUT.PostRedisplay
    end
  end

  def reshape(width = 500, height = 500)
    w, h = width.to_f, height.to_f
    GL.Viewport(0.0, 0.0, w, h)
    GL.MatrixMode(GL::PROJECTION)
    GL.LoadIdentity
    GLU.Ortho2D(0.0, 0.0, w, h)
    @winsize = width, height
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
    x = ((@dim + 1) * mx / @winsize[0]).floor
    y = ((@dim + 1) * my / @winsize[1]).floor

    if (x > @dim - 1) then x = @dim - 1 end
    if (y > @dim - 1) then y = @dim - 1 end
    if (x < 0)        then x = 0 end
    if (y < 0)        then y = 0 end

    dx = mx - @mousepos[0]
    dy = my - @mousepos[1]
    length = Math.sqrt(dx**2 + dy**2)
    if (length > 0)
      dx = dx * (0.1 / length)
      dy = dy * (0.1 / length)
    end
    
    @sim.inject_fluid(x, y, dx, dy)
    @mousepos = mx, my
  end
end

$frozen = false
Thread.abort_on_exception = true

sim = Simulation.new

viz = Visualization.new(sim)
viz.start

