<?
error_reporting(E_ALL | E_STRICT);

Ice_loadProfileWithArgs($argv);

class BI extends Test_B
{
    function ice_postUnmarshal()
    {
        $this->_postUnmarshalInvoked = true;
    }

    function postUnmarshalInvoked()
    {
        return $this->_postUnmarshalInvoked;
    }

    var $_postUnmarshalInvoked = false;
}

class CI extends Test_C
{
    function ice_postUnmarshal()
    {
        $this->_postUnmarshalInvoked = true;
    }

    function postUnmarshalInvoked()
    {
        return $this->_postUnmarshalInvoked;
    }

    var $_postUnmarshalInvoked = false;
}

class DI extends Test_D
{
    function ice_postUnmarshal()
    {
        $this->_postUnmarshalInvoked = true;
    }

    function postUnmarshalInvoked()
    {
        return $this->_postUnmarshalInvoked;
    }

    var $_postUnmarshalInvoked = false;
}

class MyObjectFactory implements Ice_ObjectFactory
{
    function create($id)
    {
        if($id == "::Test::B")
        {
            return new BI();
        }
        else if($id == "::Test::C")
        {
            return new CI();
        }
        else if($id == "::Test::D")
        {
            return new DI();
        }
        return null;
    }

    function destroy()
    {
    }
}

function test($b)
{
    if(!$b)
    {
        $bt = debug_backtrace();
        die("\ntest failed in ".$bt[0]["file"]." line ".$bt[0]["line"]."\n");
    }
}

function allTests()
{
    global $ICE;

    echo "testing stringToProxy... ";
    flush();
    $ref = "initial:default -p 12345 -t 2000";
    $base = $ICE->stringToProxy($ref);
    test($base != null);
    echo "ok\n";

    echo "testing checked cast... ";
    flush();
    $initial = $base->ice_checkedCast("::Test::Initial");
    test($initial != null);
    test($initial == $base);
    echo "ok\n";

    echo "getting B1... ";
    flush();
    $b1 = $initial->getB1();
    test($b1 != null);
    echo "ok\n";

    echo "getting B2... ";
    flush();
    $b2 = $initial->getB2();
    test($b2 != null);
    echo "ok\n";

    echo "getting C... ";
    flush();
    $c = $initial->getC();
    test($c != null);
    echo "ok\n";

    echo "getting D... ";
    flush();
    $d = $initial->getD();
    test($d != null);
    echo "ok\n";

    echo "checking consistency... ";
    flush();
    test($b1 !== $b2);
    test($b1 !== $c);
    test($b1 !== $d);
    test($b2 !== $c);
    test($b2 !== $d);
    test($c !== $d);
    test($b1->theB != null);
    test($b1->theB === $b1);
    test($b1->theC == null);
    test($b1->theA != null);
    test($b1->theA->theA === $b1->theA);
    test($b1->theA->theB === $b1);
    test($b1->theA->theC != null);
    test($b1->theA->theC->theB === $b1->theA);
    test($b1->preMarshalInvoked);
    test($b1->postUnmarshalInvoked());
    test($b1->theA->preMarshalInvoked);
    test($b1->theA->postUnmarshalInvoked());
    test($b1->theA->theC->preMarshalInvoked);
    test($b1->theA->theC->postUnmarshalInvoked());
    // More tests possible for b2 and d, but I think this is already sufficient.
    test($b2->theA === $b2);
    test($d->theC == null);
    echo "ok\n";

    //
    // Break cyclic dependencies
    //
    $b1->theA->theA = null;
    $b1->theA->theB = null;
    $b1->theA->theC = null;
    $b1->theA = null;
    $b1->theB = null;

    $b2->theA = null;
    $b2->theB->theA = null;
    $b2->theB->theB = null;
    $b2->theC = null;

    $c->theB->theA = null;
    $c->theB->theB->theA = null;
    $c->theB->theB->theB = null;
    $c->theB = null;

    $d->theA->theA->theA = null;
    $d->theA->theA->theB = null;
    $d->theA->theB->theA = null;
    $d->theA->theB->theB = null;
    $d->theB->theA = null;
    $d->theB->theB = null;
    $d->theB->theC = null;

    echo "getting B1, B2, C, and D all at once... ";
    flush();
    $initial->getAll($b1, $b2, $c, $d);
    test($b1 != null);
    test($b2 != null);
    test($c != null);
    test($d != null);
    echo "ok\n";

    echo "checking consistency... ";
    flush();
    test($b1 !== $b2);
    test($b1 !== $c);
    test($b1 !== $d);
    test($b2 !== $c);
    test($b2 !== $d);
    test($c !== $d);
    test($b1->theA === $b2);
    test($b1->theB === $b1);
    test($b1->theC == null);
    test($b2->theA === $b2);
    test($b2->theB === $b1);
    test($b2->theC === $c);
    test($c->theB === $b2);
    test($d->theA === $b1);
    test($d->theB === $b2);
    test($d->theC == null);
    test($d->preMarshalInvoked);
    test($d->postUnmarshalInvoked());
    test($d->theA->preMarshalInvoked);
    test($d->theA->postUnmarshalInvoked());
    test($d->theB->preMarshalInvoked);
    test($d->theB->postUnmarshalInvoked());
    test($d->theB->theC->preMarshalInvoked);
    test($d->theB->theC->postUnmarshalInvoked());
    echo "ok\n";

    //
    // Break cyclic dependencies
    //
    $b1->theA = null;
    $b1->theB = null;
    $b2->theA = null;
    $b2->theB = null;
    $b2->theC = null;
    $c->theB = null;
    $d->theA = null;
    $d->theB = null;

    return $initial;
}

$factory = new MyObjectFactory();
$ICE->addObjectFactory($factory, "::Test::B");
$ICE->addObjectFactory($factory, "::Test::C");
$ICE->addObjectFactory($factory, "::Test::D");
$initial = allTests();
$initial->shutdown();
exit();
?>
