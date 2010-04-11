/*
 *  Test strict mode
 */
use strict;

module test
{
    public class A {
        public var foo: String = "bar";
    }

    public class B {
        public function accessFoo(a: A): void {
            //  NOTE: This statement was failing with "foo" not found
            a.foo = "blabla";
            print(a.foo);
        }
    }
}
