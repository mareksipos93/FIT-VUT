require_relative 'fib-sequence' # totez i pro fib-sequence_slow

class Exec
  @fibObject = FibonacciSequence.new
  20.times { @fibObject.next }
end