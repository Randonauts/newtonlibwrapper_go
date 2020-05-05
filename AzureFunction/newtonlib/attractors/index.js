const { spawn } = require('promisify-child-process')

module.exports = async function (context, req) {
    context.log(`/attractors request: -radius ${req.query.radius ? req.query.radius : '<nil>'} -gid ${req.query.gid ? req.query.gid : '<nil>'} | entropy.length ${req.body ? req.body.length : '<nil>'}`);

        try {
            // $ func azure functionapp publish newtonlib
            // The above command had a bug in that it didn't preserver the chmod +x executable permissions
            // so we manually do it here... ugly. Not as ugly as the fact I'm wrapping a go wrapper in a nodejs wrapper.
            // But hey my goal was to hack a bit with Go and Azure Functions and I got there :)
            await spawn('chmod', [ '+x', './bin/newtonlibwrapper_go' ], {});

            const child = spawn('./bin/newtonlibwrapper_go',
            [
                `-radius=${req.query.radius}`,
                `-latitude=${req.query.latitude}`,
                `-longitude=${req.query.longitude}`,
                `-gid=${req.query.gid}`,
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
