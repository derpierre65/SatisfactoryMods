// Google Cloud TTS generation (voice en-US-Wavenet-C, pitch -1.2)
// Usage: node generate_ada_message.js output.wav (text read from input.txt)
// Needs: GOOGLE_TTS_API_KEY env key

const { writeFile, readFile } = require('node:fs/promises');
const { join } = require('node:path');
require('dotenv').config();

async function main() {
    const API_KEY = process.env.GOOGLE_TTS_API_KEY;
    if (!API_KEY) {
        console.error('Missing GOOGLE_TTS_API_KEY env var');
        process.exit(1);
    }

    const text = (await readFile(join(__dirname, 'input.txt'), 'utf8')).trim();
    if (!text) {
        console.error('input.txt is empty');
        process.exit(1);
    }

    const outFile = process.argv[2] ?? 'output.wav';

    const body = {
        input: {
            ssml: text,
        },
        voice: {
            languageCode: 'en-US',
            name: 'en-US-Wavenet-C',
        },
        audioConfig: {
            audioEncoding: 'LINEAR16',
            pitch: -1.2,
            speakingRate: 1.0,
        },
    };

    const res = await fetch(
        `https://texttospeech.googleapis.com/v1/text:synthesize?key=${API_KEY}`,
        {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(body),
        }
    );

    if (!res.ok) {
        console.error('TTS failed:', res.status, await res.text());
        process.exit(1);
    }

    const { audioContent } = await res.json();
    await writeFile(outFile, Buffer.from(audioContent, 'base64'));
    console.log(`Wrote ${outFile}, Done.`);
}

main();