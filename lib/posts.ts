import fs from "fs"
import { join } from "path"
import matter from 'gray-matter'

const postsDirectory = join(process.cwd(), "posts");

export function getPostSlugs() {
  const fileNames = fs.readdirSync(postsDirectory);
  return fileNames.map(filename => filename.replace(/\.mdx$/, ""))
}

export function getPostBySlug(slug: string, fields: string[] = []) {
  const fullPath = join(postsDirectory, `${slug}.mdx`);
  const fileContents = fs.readFileSync(fullPath, "utf8");
  const { data, content } = matter(fileContents)

  type Items = {
    [key: string]: string;
  }

  const items: Items = {};

  fields.forEach((field) => {
    if (field === "slug") {
      items[field] = slug;
    } else if (field === "content") {
      items[field] = content;
    } else if (typeof data[field] !== undefined) {
      items[field] = data[field];
    }
  });

  return items;
}

export function getAllPosts(fields: string[] = []) {
  const slugs = getPostSlugs();
  const posts = slugs.map((slug) => getPostBySlug(slug, fields));
  return posts;
}
