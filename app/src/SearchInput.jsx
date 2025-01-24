import React from 'react';

export default function SearchInput(props) {
    return (
        <div className='searchContainer'>
            <input
                type="text"
                id="search"
                name="word"
                className='searchInput'
                placeholder="Search for words with similar vibes..."
                spellCheck={false}
            />
        </div>
    );
}