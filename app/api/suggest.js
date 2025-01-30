export default async function handler(req, res) {
    const { prefix, n } = req.query;
    const apiUrl = process.env.VITE_API_URL;

    try {
        const response = await fetch(`${apiUrl}/suggest?prefix=${prefix}&n=${n}`);
        const data = await response.json();
        res.status(200).json(data);
    } catch (error) {
        console.log(error);
        res.status(500).json({ error: 'Failed to fetch suggestions.' });
    }
}