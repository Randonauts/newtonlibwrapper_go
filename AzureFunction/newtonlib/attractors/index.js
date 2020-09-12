const { spawn } = require('promisify-child-process')

module.exports = async function (context, req) {
    context.log(`/attractors request`);

    try {
        // $ func azure functionapp publish newtonlib
        // The above command had a bug in that it didn't preserve the chmod +x executable permissions
        // so we manually do it here... ugly. Not as ugly as the fact I'm wrapping a go wrapper in a nodejs wrapper.
        // But hey my goal was to hack a bit with Go and Azure Functions and I got there :)
        await spawn('chmod', [ '+x', './bin/newtonlibwrapper_go' ], {});

        const child = spawn('./bin/newtonlibwrapper_go',
        [
            `-pointType=${req.query.t}`,
            `-radius=3000`,
            `-latitude=${req.query.l1}`,
            `-longitude=${req.query.l2}`,
            `-gid=23`,
            `-server=false`
        ],
        {
            encoding: "binary"
        });

        child.stdin.write(req.body);
        child.stdin.end();

        const { stdout, stderr, code } = await child;

        var out = stdout.toString();
        var err = stderr.toString();
        if (err) {
            context.res = {
                status: 400,
                body: JSON.stringify({
                            error: err,
                            stdout: out
                        })
            };
            return;
        }

        context.res = {
            status: 200,
            body: out
        };
    } catch (err) {
        context.res = {
            status: 500,
            body: JSON.stringify({
                        error: "Something goes wrong",
                        details: err
                    })
        };
    }
};
