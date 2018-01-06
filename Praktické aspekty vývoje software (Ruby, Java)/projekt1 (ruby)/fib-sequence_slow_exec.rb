require_relative 'fib-sequence_slow'

class Exec
  @fibObject = FibonacciSequence.new
  20.times { @fibObject.next }
end